#include <iostream>
#include <string.h>
#include "../jsapi/include/jsapi.h"
#include "../jsapi/include/jstypes.h"
#include "../jsapi/include/jsstddef.h"
#include "../jsapi/include/jsatom.h"

#include "shell.h"
#include "Printer.h"

using namespace std;

JSBool gQuitting = JS_FALSE;

static JSBool
Help(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

static JSBool
Quit(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
#ifdef LIVECONNECT
    JSJ_SimpleShutdown();
#endif

    JS_ConvertArguments(cx, argc, argv,"/ i", 0);

    gQuitting = JS_TRUE;
    return JS_FALSE;
}

static JSBool
Version(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    if (argc > 0 && JSVAL_IS_INT(argv[0]))
        *rval = INT_TO_JSVAL(JS_SetVersion(cx, (JSVersion) JSVAL_TO_INT(argv[0])));
    else
        *rval = INT_TO_JSVAL(JS_GetVersion(cx));
    return JS_TRUE;
}

static const char shell_help_header[] =
"Command                  Description\n"
"=======                  ===========\n";

static JSFunctionSpec shell_functions[] = {
    JS_FS("version",        Version,        0,0,0),
    JS_FS("help",           Help,           0,0,0),
    JS_FS("quit",           Quit,           0,0,0)
};

static const char *const shell_help_messages[] = {
    "version([number])        Get or set JavaScript version number",
    "help([name ...])         Display usage and help messages",
    "quit()                   Quit the shell"
};

static void Process(JSContext *cx, JSObject *obj){
    JSBool ok;
    JSBool hitEOF = JS_FALSE;
    JSScript *script;
    jsval result;
    JSString *str;
    char buf[4096];
    int lineNo = 1;
    int buflen = 0;
    int startline=0;

    do {
        fprintf(stdout,"sh>");
        fflush(stdout);
        do {
            if (!fgets(buf + buflen, sizeof(buf) - buflen - 1, stdin))
			{
				hitEOF = JS_FALSE;
				break;
			}
			buflen += strlen(buf + buflen);
			lineNo++;
        } while (!JS_BufferIsCompilableUnit(cx, obj, buf, strlen(buf)));

        /* Clear any pending exception from previous failed compiles.  */
        JS_ClearPendingException(cx);
        script = JS_CompileScript(cx, obj, buf, strlen(buf), "typein",
                                  startline);
        if (script) {
            //if (!compileOnly) {
            ok = JS_ExecuteScript(cx, obj, script, &result);
            if (ok && result != JSVAL_VOID) {
                str = JS_ValueToString(cx, result);
                if (str)
                    fprintf(stdout, "%s\n", JS_GetStringBytes(str));
                else
                    ok = JS_FALSE;
            }
            //}
            JS_DestroyScript(cx, script);
        }
        buflen = 0;
    } while (!hitEOF && !gQuitting);
}

int main()
{
    Printer output = Printer(stdout);
    Printer errput = Printer(stdout);

    shell sh = shell(&output, &errput, 10 * 1024 * 1024, 8192);
    JSContext* cx = sh.GetJSContext();
    JSObject* obj = sh.GetJSObject();

    if (!JS_DefineFunctions(cx, obj, shell_functions)){
        return 1;
    }

    Process(cx, obj);

    return 0;
}

static JSBool
Help(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    uintN i, j;
    int did_header, did_something;
    JSType type;
    JSString *str;
    const char *bytes;

    fprintf(stdout, "%s\n", JS_GetImplementationVersion());
    if (argc == 0) {
        fputs(shell_help_header, stdout);
        for (i = 0; shell_functions[i].name; i++)
            fprintf(stdout, "%s\n", shell_help_messages[i]);
    } else {
        did_header = 0;
        for (i = 0; i < argc; i++) {
            did_something = 0;
            type = JS_TypeOfValue(cx, argv[i]);
            if (type == JSTYPE_STRING) {
                str = JSVAL_TO_STRING(argv[i]);
            } else {
                str = NULL;
            }
            if (str) {
                bytes = JS_GetStringBytes(str);
                for (j = 0; shell_functions[j].name; j++) {
                    if (!strcmp(bytes, shell_functions[j].name)) {
                        if (!did_header) {
                            did_header = 1;
                            fputs(shell_help_header, stdout);
                        }
                        did_something = 1;
                        fprintf(stdout, "%s\n", shell_help_messages[j]);
                        break;
                    }
                }
            }
            if (!did_something) {
                str = JS_ValueToString(cx, argv[i]);
                if (!str)
                    return JS_FALSE;
                fprintf(stderr, "Sorry, no help for %s\n",
                        JS_GetStringBytes(str));
            }
        }
    }
    printf("help\n");
    return JS_TRUE;
}
