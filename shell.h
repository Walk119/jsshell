#ifndef SHELL_H_INCLUDED
#define SHELL_H_INCLUDED

#include "../jsapi/include/jsapi.h"
#include "../jsapi/include/jstypes.h"
#include "Printer.h"

#include <list>
#include <string>

class shell{
public:
    shell(Printer* output, Printer* errput, uint32 maxBytes, size_t stackChunkSize);
    ~shell();
    JSContext* GetJSContext();
    JSObject* GetJSObject();
    JSBool execScript(jsval* rval, const char* script, int len, int lineNo, const char* filenane);
    void loadDLL(char* filename);
    void listDLL();
private:

private:
    std::list<char*> funcList;
    JSRuntime* jsRT;
    JSContext* jsCX;
    JSObject* jsOBJ;
};

static void my_ErrorReporter(JSContext* cx, const char* message, JSErrorReport *report);
static JSBool global_enumerate(JSContext* cx, JSObject *obj);
static JSBool global_resolve(JSContext *cx, JSObject *obj, jsval id, uintN flags, JSObject **objp);
void SetContextOptions(JSContext *cx);
static JSBool ContextCallback(JSContext *cx, uintN contextOp);
#endif // SHELL_H_INCLUDED
