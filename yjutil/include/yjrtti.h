#ifndef YJRTTI_H_
#define YJRTTI_H_

namespace yjutil {

/**
 * @addtogroup rtti
 * @{
 */

/**
 * @brief The root class of dynamic-type
 */
class _Object;

/**
 * @brief The runtime_class for dynamic ability
 */
struct _Runtime_Class {
    const char *m_lpClassName;
    int m_nObjectSize;
    unsigned int m_wSchema;
    _Object *(_stdcall *m_pfnCreateObject)();
#ifdef _AFXDLL
    _Runtime_Class *(_stdcall *m_pfnGetBaseClass)();
#else
    _Runtime_Class *m_pBaseClass;
#endif
    // CObject* CreateObject();
    _Runtime_Class *m_pNextClass;
    // const AFX_CLASSINIT* m_pClassInit;
};

/// Declare the dynamic class and func
#define DECLARE_DYNAMIC(class_name)                \
  public:                                          \
    static const _Runtime_Class class##class_name; \
    virtual _Runtime_Class *GetRuntimeClass() const;

/// Define the dynamic class and fun
/// @see IMPLEMENT_RUNTIMECLASS
#define IMPLEMENT_DYNAMIC(class_name, base_class) \
    IMPLEMENT_RUNTIMECLASS(class_name, base_class, 0xFFFF, NULL, NULL)

/// Define the dynamic class and fun
/// @see IMPLEMENT_DYNAMCI
#define IMPLEMENT_RUNTIMECLASS(class_name, base_class, wSchema, pfnNew, \
                               class_init)                              \
    const _Runtime_Class class_name::class##class_name = {              \
        #class_name, sizeof(class class_name), wSchema,                 \
        pfnNew, RUNTIME_CLASS(base_class), NULL};                       \
    _Runtime_Class *class_name::GetRuntimeClass() const {               \
        return RUNTIME_CLASS(class_name);                               \
    }

/// Get the runtime class
/// @see _RUNTIME_CLASS
#define RUNTIME_CLASS(class_name) _RUNTIME_CLASS(class_name)

/// Get the runtime class
/// @see RUNTIME_CLASS
#define _RUNTIME_CLASS(class_name) \
    ((_Runtime_Class *)(&class_name::class##class_name))

/**
 * @brief The root class of dynamic-class
 */
class _Object {
    DECLARE_DYNAMIC(_Object)
  public:
  private:
};
const _Runtime_Class _Object::class_Object = {
    "CObject", sizeof(_Object), 0xffff, NULL, NULL, NULL};
_Runtime_Class *_Object::GetRuntimeClass() const {
    return _RUNTIME_CLASS(_Object);
}

/**
 * @}
 */

} // namespace yjutil

#endif