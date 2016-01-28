
#include "jrep.h"

#include <iostream>

using namespace std;

int main() {

    JObjectPtr obj = make_shared<JObject>();
    JObjectPtr obj1= make_shared<JObject>();
    JStringPtr str = make_shared<JString>("just a stupid string");
    JNumberPtr num = make_shared<JNumber>(3.1415);
    JNumberPtr num2= make_shared<JNumber>(3.1415 * 2);
    JNumberPtr num3= make_shared<JNumber>(-2);
    JNumberPtr num4= make_shared<JNumber>(-2 * 10e20);
    JNumberPtr num5= make_shared<JNumber>(2);
    JNumberPtr num6= make_shared<JNumber>(0.45 / 10e20);
    JArrayPtr  ar  = make_shared<JArray>();
    JBoolPtr   b   = make_shared<JBool>(true);
    JBoolPtr   b1  = make_shared<JBool>(false);
    JNullPtr   n   = make_shared<JNull>();

    // order matters, if array is put in object first
    // and then obj1 added to obj the depth is not perfect
    obj->add_value("embedded object", obj1);
    obj1->add_value("Array", JPtr(ar));

    ar->add_value(JPtr(num));
    ar->add_value((num2));
    ar->add_value((num3));
    ar->add_value((num4));
    ar->add_value((num5));
    ar->add_value((num6));
    
    obj1->add_value("size", make_shared<JNumber>(ar->size()));
    
    obj->add_value("string", str);
    obj->add_value("b", b);
    obj->add_value("false", b1);
    obj->add_value("", n);
    obj->add_value("0", n);
    
    /* Oops embedding object (directly or) indirectly in it self.
     * this will lead to infinite recursion in obj->representation
     * and end in a stack overflow or in a infinite loop.
     */
    // ar->add_value(JPtr(obj)); 

    cout << obj->representation() << endl;
}
