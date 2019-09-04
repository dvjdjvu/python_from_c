/**
 * gcc class.c $(python3-config --includes --ldflags) -o class && ./class
 * https://docs.python.org/3/c-api/arg.html#c.Py_BuildValue Building values
 */

#include <Python.h>
#include <stdio.h>
#include <stdbool.h>

PyObject *pName = NULL, *pModule = NULL, *pInstance = NULL;
PyObject *pDict = NULL, *pClass = NULL, *pVal = NULL;
PyObject* sys = NULL;
PyObject* sys_path = NULL;
PyObject* folder_path = NULL;

/*
 * Загрузка интерпритатора python и модуля class.py в него.
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
        pName = PyUnicode_FromString("class");
        if (!pName) {
            break;
        }

        // Загрузить модуль class
        pModule = PyImport_Import(pName);
        if (!pModule) {
            break;
        }

        // Словарь объектов содержащихся в модуле
        pDict = PyModule_GetDict(pModule);
        if (!pDict) {
            break;
        }

        // Загрузка объекта Class из class.py
        pClass = PyDict_GetItemString(pDict, (const char *) "Class");
        if (!pClass) {
            break;
        }

        // Проверка pClass на годность.
        if (!PyCallable_Check(pClass)) {
            break;
        }

        // Указатель на Class
        pInstance = PyObject_CallObject(pClass, NULL);

        return pInstance;
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
    Py_XDECREF(pInstance);
    Py_XDECREF(pClass);
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
python_class_get_str(char *val) {
    char *ret = NULL;

    pVal = PyObject_CallMethod(pInstance, (char *) "get_value", (char *) "(s)", val);
    if (pVal != NULL) {
        PyObject* pResultRepr = PyObject_Repr(pVal);
        
        // Если полученную строку не скопировать, то после очистки ресурсов python её не будет.
        ret = strdup(PyBytes_AS_STRING(PyUnicode_AsEncodedString(pResultRepr, "utf-8", "ERROR")));
        
        Py_XDECREF(pResultRepr);
        Py_XDECREF(pVal);
    } else {
        PyErr_Print();
    }

    return ret;
}

/**
 * Передача bool в качестве аргумента и получение bool назад
 */
bool
python_class_get_bool(bool val) {
    bool ret = false;

    //printf("val %d\n", val);
    pVal = PyObject_CallMethod(pInstance, (char *) "get_bool", (char *) "(i)", val);
    if (pVal != NULL) {
        if (PyBool_Check(pVal)) {
            ret = PyObject_IsTrue(pVal);
        }
        
        Py_XDECREF(pVal);
    } else {
        PyErr_Print();
    }

    return ret;
}

/**
 * Передача int в качестве аргумента и получение int назад
 */
int 
python_class_get_int(int val) {
    int ret = 0;

    pVal = PyObject_CallMethod(pInstance, (char *) "get_value", (char *) "(i)", val);
    if (pVal != NULL) {
        if (PyLong_Check(pVal)) {
            ret = _PyLong_AsInt(pVal);
        }
        
        Py_XDECREF(pVal);
    } else {
        PyErr_Print();
    }

    return ret;
}

/**
 * Передача double в качестве аргумента и получение double назад
 */
double 
python_class_get_double(double val) {
    double ret = 0.0;

    pVal = PyObject_CallMethod(pInstance, (char *) "get_value", (char *) "(f)", val);
    if (pVal != NULL) {
        if (PyFloat_Check(pVal)) {
            ret = PyFloat_AS_DOUBLE(pVal);
        }
        
        Py_XDECREF(pVal);
    } else {
        PyErr_Print();
    }

    return ret;
}

/**
 * Получение значения переменной содержащей значение типа int
 */
int 
python_class_get_val(char *val) {
    int ret = 0;

    pVal = PyObject_GetAttrString(pInstance, (char *) val);
    if (pVal != NULL) {
        if (PyLong_Check(pVal)) {
            ret = _PyLong_AsInt(pVal);
        }
        
        Py_XDECREF(pVal);
    } else {
        PyErr_Print();
    }

    return ret;
}

int
main() {
    puts("Test class:");
    
    if (!python_init()) {
        puts("python_init error");
        return -1;
    }
    
    puts("Strings:");
    printf("\tString: %s\n", python_class_get_str("Hello from Python!"));
    
    puts("Bools:");
    printf("\tbool: %s\n", python_class_get_bool(false) ? "true" : "false");
    printf("\tbool: %s\n", python_class_get_bool(true) ? "true" : "false");

    puts("Digits:");
    printf("\tint: %d\n", python_class_get_int(32));
    printf("\tdouble: %f\n", python_class_get_double(23.123456789));
    
    puts("Class Attrs:");
    printf("\ta: %d\n", python_class_get_val("a"));
    printf("\tb: %d\n", python_class_get_val("b"));
    printf("\tc: %d\n", python_class_get_val("c"));
    
    python_clear();
    
    return 0;
}