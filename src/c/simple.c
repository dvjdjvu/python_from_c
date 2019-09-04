/**
 * gcc simple.c $(python3-config --includes --ldflags) -o simple && ./simple
 */

#include <Python.h>

void 
python() {
    // Загрузка интерпритатора Python
    Py_Initialize();
    
    // Выполнение команды в интерпритаторе
    PyRun_SimpleString("import sys");
    // Путь до наших исходников python
    PyRun_SimpleString("sys.path.append('./src/python')");
    PyRun_SimpleString("import simple");
    PyRun_SimpleString("print(simple.get_value(2))");
    PyRun_SimpleString("print(simple.get_value(2.0))");
    PyRun_SimpleString("print(simple.get_value(\"Hello!\"))");
    
    // Выгрузка интерпритатора Python
    Py_Finalize();  
}

void
main() {
    puts("Test simple:");
    
    python();
}
