/**
 * gcc func.c $(python3-config --includes --ldflags) -o func && ./func
 */

#include <Python.h>

PyObject *pName = NULL, *pModule = NULL;
PyObject *pDict = NULL, *pObjct = NULL, *pVal = NULL;
PyObject* sys = NULL;
PyObject* sys_path = NULL;
PyObject* folder_path = NULL;

/*
 * Загрузка интерпритатора python и модуля func.py в него.
 */
PyObject *
python_init() {
    // Инициализировать интерпретатор Python
    Py_Initialize();

    do {
        // Загрузка модуля sys
        sys = PyImport_ImportModule("sys");
        sys_path = PyObject_GetAttrString(sys, "path");
        // Путь до наших исходников python
        folder_path = PyUnicode_FromString((const char*) "./src/python");
        PyList_Append(sys_path, folder_path);

        // Создание Unicode объекта из UTF-8 строки
        pName = PyUnicode_FromString("func");
        if (!pName) {
            break;
        }

        // Загрузить модуль func
        pModule = PyImport_Import(pName);
        if (!pModule) {
            break;
        }

        // Словарь объектов содержащихся в модуле
        pDict = PyModule_GetDict(pModule);
        if (!pDict) {
            break;
        }

        return pDict;
    } while (0);

    // Печать ошибки
    PyErr_Print();
}

/*
 * Освобождение ресурсов интерпритатора python
 */
void
python_clear() {
    // Вернуть ресурсы системе
    Py_XDECREF(pDict);
    Py_XDECREF(pModule);
    Py_XDECREF(pName);

    Py_XDECREF(folder_path);
    Py_XDECREF(sys_path);
    Py_XDECREF(sys);
    
    // Выгрузка интерпритатора Python
    Py_Finalize();
}

/**
 * Передача строки в качестве аргумента и получение строки назад
 */
char *
python_func_get_str(char *val) {
    char *ret = NULL;

    // Загрузка объекта get_value из func.py
    pObjct = PyDict_GetItemString(pDict, (const char *) "get_value");
    if (!pObjct) {
        return ret;
    }

    do {
        // Проверка pObjct на годность.
        if (!PyCallable_Check(pObjct)) {
            break;
        }

        pVal = PyObject_CallFunction(pObjct, (char *) "(s)", val);
        if (pVal != NULL) {
            PyObject* pResultRepr = PyObject_Repr(pVal);

            // Если полученную строку не скопировать, то после очистки ресурсов python её не будет.
            // Для начала pResultRepr нужно привести к массиву байтов.
            ret = strdup(PyBytes_AS_STRING(PyUnicode_AsEncodedString(pResultRepr, "utf-8", "ERROR")));

            Py_XDECREF(pResultRepr);
            Py_XDECREF(pVal);
        } else {
            PyErr_Print();
        }
    } while (0);

    Py_XDECREF(pObjct);

    return ret;
}

/**
 * Получение значения переменной содержащей значение типа int
 */
int
python_func_get_val(char *val) {
    int ret = 0;

    // Получить объект с именем val
    pVal = PyDict_GetItemString(pDict, (const char *) val);
    if (!pVal) {
        return ret;
    }
    
    // Проверка переменной на long
    if (PyLong_Check(pVal)) {
        ret = _PyLong_AsInt(pVal);
    } else {
        PyErr_Print();
    }

    Py_XDECREF(pVal);

    return ret;
}

int
main() {
    puts("Test func:");

    if (!python_init()) {
        puts("python_init error");
        return -1;
    }

    puts("Strings:");
    printf("\tString: %s\n", python_func_get_str("Hello from Python!"));
    
    puts("Attrs:");
    printf("\ta: %d\n", python_func_get_val("a"));
    printf("\tb: %d\n", python_func_get_val("b"));
    printf("\tc: %d\n", python_func_get_val("c"));

    python_clear();

    return 0;
}
