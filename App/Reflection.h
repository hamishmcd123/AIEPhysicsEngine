#pragma once 
#include <memory>
#include <vector>
#include "Vec2.h"
#include "imgui.h"
#include <string>
#include <cstdint>

// Useful Macros
#define BEGIN_REFLECTION(TYPE) \
    static void RegisterClass() { \
    using SelfType = TYPE; \
    TypeInfo<TYPE>& info = GetType<TYPE>(); \

#define REFLECT(PROPERTY) \
        info.properties.push_back(make_property(#PROPERTY, &SelfType::PROPERTY)); 

#define END_REFLECTION }

template <typename Class>
class IProperty{
    public:
    const char* name;
    virtual void Draw(Class* instance) = 0;
    virtual ~IProperty() = default;
};

template <typename Class, typename T>
class Property : public IProperty<Class> {
    public:
    T Class::* member;

    Property(const char* _name, T Class::* m) : member(m){
       this->name = _name;
    }

    void Draw(Class* instance) override {
        DrawImpl(instance->*member, instance);
    }

    private:
    
    std::string ImGUIDHelper(Class* instance) {
        return "##" + std::string(this->name) + "_" + std::to_string(reinterpret_cast<uintptr_t>(instance));

    }

    void DrawImpl(Vec2& vec2, Class* instance) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text(this->name);
        ImGui::TableNextColumn(); ImGui::InputFloat2(ImGUIDHelper(instance).c_str(), &vec2.x);
    }
    
    void DrawImpl(float& flt, Class* instance) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text(this->name);
        ImGui::TableNextColumn(); ImGui::InputFloat(ImGUIDHelper(instance).c_str(), &flt);
    }

    void DrawImpl(const char* string, Class* instance) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text(this->name);
        ImGui::TableNextColumn(); ImGui::Text(string);
    }

};

// CTAD deduction guide for property constructor
template<typename Class, typename T>
Property(const char*, T Class::* m) -> Property<Class,T>;

// Helper function for std::unique_ptr
template<typename Class, typename T>
std::unique_ptr<IProperty<Class>> make_property(const char* name, T Class::* m) {
    return std::make_unique<Property<Class, T>>(name, m);
}

template<typename Class>
struct TypeInfo{
    const char* name;
    std::vector<std::unique_ptr<IProperty<Class>>> properties;
};

template <typename Class>
TypeInfo<Class>& GetType() {
    // TODO: typeid is implementation-specific...
    static TypeInfo<Class> typeInfo {typeid(Class).name()};
    return typeInfo;
}
