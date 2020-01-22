#include "shell.h"


shell::shell(Printer* output, Printer* errput, uint32 maxBytes = 0, size_t stackChunkSize = 0)
{
    this->jsRT = JS_NewRuntime(maxBytes);
    if (!this->jsRT)
        return;

    this->jsCX = JS_NewContext(this->jsRT, stackChunkSize);
    if (!this->jsCX)
        return;

    this->jsOBJ = JS_NewObject(this->jsCX, &this->global_class, NULL, NULL);
    if (!this->jsOBJ)
        return;

    JS_SetGlobalObject(this->jsCX, this->jsOBJ);
}


JSContext* shell::GetJSContext()
{
    return this->jsCX;
}

JSObject* shell::GetJSObject()
{
    return this->jsOBJ;
}

shell::~shell()
{
    JS_DestroyContext(this->jsCX);
    JS_DestroyRuntime(this->jsRT);
    JS_ShutDown();
}

static JSBool global_enumerate(JSContext* cx, JSObject *obj)
{
    return JS_EnumerateStandardClasses(cx, obj);
}

static JSBool global_resolve(JSContext *cx, JSObject *obj, jsval id, uintN flags, JSObject **objp)
{
    if ((flags & JSRESOLVE_ASSIGNING) == 0){
        JSBool resolved;
        if (!JS_ResolveStandardClass(cx, obj, id, &resolved))
            return JS_FALSE;
        if (resolved) {
            *objp = obj;
            return JS_TRUE;
        }
    }
    else{
        return JS_TRUE;
    }
}

static void my_ErrorReporter(JSContext* cx, const char* message, JSErrorReport *report)
{
    printf("%s",message);
}

void SetContextOptions(JSContext* cx)
{
    jsuword stackLimit = 0;
    JS_SetThreadStackLimit(cx, stackLimit);
    JS_SetScriptStackQuota(cx, JS_DEFAULT_SCRIPT_STACK_QUOTA);
}

static JSBool ContextCallback(JSContext *cx, uintN contextOp)
{
    if (contextOp == JSCONTEXT_NEW) {
        JS_SetErrorReporter(cx, my_ErrorReporter);
        JS_SetVersion(cx, JSVERSION_LATEST);
        SetContextOptions(cx);
    }
    return JS_TRUE;
}

