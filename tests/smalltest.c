
#include <stdlib.h>
#include "jrep.h"

#include <stdio.h>


int main() {

    j_object* obj = j_object_create();
    j_object* obj1= j_object_create();
    j_string* str = j_string_create_str("just a stupid string");
    j_number* num = j_number_create(3.1415);
    j_number* num2= j_number_create(3.1415 * 2);
    j_number* num3= j_number_create(-2);
    j_number* num4= j_number_create(-2 * 10e20);
    j_number* num5= j_number_create(2);
    j_number* num6= j_number_create(0.45 / 10e20);
    j_array*  ar  = j_array_create();
    j_bool*   b   = j_bool_create_bool(1);
    j_bool*   b1  = j_bool_create();
    j_null*   n   = j_null_create();

    // order matters, if array is put in object first
    // and then obj1 added to obj the depth is not perfect
    j_object_add_value(obj, "embedded object", (j_val*) obj1);
    j_object_add_value(obj1, "Array", (j_val*)ar);

    j_array_add_value(ar, (j_val*) num);
    j_array_add_value(ar, (j_val*)num2);
    j_array_add_value(ar, (j_val*)num3);
    j_array_add_value(ar, (j_val*)num4);
    j_array_add_value(ar, (j_val*)num5);
    j_array_add_value(ar, (j_val*)num6);
    
    j_object_add_value(obj1, "size", (j_val*)j_number_create(j_array_size(ar)));
    
    j_object_add_value(obj, "string", (j_val*)str);
    j_object_add_value(obj, "b",(j_val*) b);
    j_object_add_value(obj, "false", (j_val*)b1);
    j_object_add_value(obj, "", (j_val*)n);
    j_object_add_value(obj, "0", (j_val*)n);
    
    /* Oops embedding object (directly or) indirectly in it self.
     * this will lead to infinite recursion in obj->representation
     * and end in a stack overflow or in a infinite loop.
     */
    // ar->add_value(JPtr(obj)); 

    j_object_destroy(obj1);
    j_string_destroy(str);
    j_number_destroy(num);
    j_number_destroy(num2);
    j_number_destroy(num3);
    j_number_destroy(num4);
    j_number_destroy(num5);
    j_number_destroy(num6);
    j_array_destroy(ar);
    j_bool_destroy(b);
    j_bool_destroy(b1);
    j_null_destroy(n);

    char* msg = j_val_representation((j_val*)obj);
    fprintf(stdout, "%s\n", msg );
    free(msg);
    j_val_destroy((j_val*)obj);
    return 0;
}
