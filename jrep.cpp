#include "jrep.h"

#include <sstream>
#include <cstring>

using std::ostringstream;
using std::string;
using std::map;
using std::deque;
using std::vector;
using std::endl;

JValue::JValue(jtype t)
    : my_type(t)
{
}

JValue::~JValue()
{
}

void JValue::set_depth(int depth)
{
    my_depth = depth;
}

int JValue::get_depth() const
{
    return my_depth;
}

jtype JValue::get_type() const
{
    return my_type;
}

JObject::JObject() :
    JValue(OBJECT)
{
}

void JObject::add_value(const string& key, JPtr value)
{
    my_pairs[key] = value;
    value->set_depth(get_depth() + 1);
}

JPtr JObject::get_value(const string& key) const
{
    map<string, JPtr>::const_iterator it;
    it = my_pairs.find(key);
    if (it == my_pairs.end())
        throw NoSuchKeyError(key);
    return it->second;
}

std::string JObject::representation() const
{
    ostringstream ret;
    ret << "{" << endl;
    map<string, JPtr>::const_iterator it = my_pairs.begin();
    for (unsigned i=0; it != my_pairs.end(); ++it, ++i) {
        for (int j = 0; j < it->second->get_depth(); ++j)
            ret << "\t";
        if (i < my_pairs.size() - 1)
            ret << "\"" << it->first << "\" : " <<
                it->second->representation() << "," << endl ;
        else
            ret << "\"" << it->first << "\" : " <<
                it->second->representation() << endl;
    }
    
    for (int j = 0; j < get_depth(); ++j)
        ret << "\t";
    ret << "}";
    return ret.str();
}

JObject::NoSuchKeyError::~NoSuchKeyError() throw()
{
}

JObject::NoSuchKeyError::NoSuchKeyError(const string& key)
{
    my_key = "No such key: " + key;
}

const char* JObject::NoSuchKeyError::what()const throw()
{
    return my_key.c_str();
}

JArray::JArray()
    : JValue(ARRAY)
{
}

void JArray::add_value(JPtr value)
{
    my_values.push_back(value);
    value->set_depth(get_depth() + 1);
}

vector<JPtr>::size_type JArray::size()
{
    return my_values.size();
}

JPtr JArray::get_value(vector<JPtr>::size_type index) const
{
    return my_values[index];
}

string JArray::representation() const
{
    ostringstream ret;
    ret << "[" << endl;
    for (unsigned i = 0; i < my_values.size(); ++i) {
        for (int j = 0; j < my_values[i]->get_depth(); ++j)
            ret << "\t";
        if (i < my_values.size() - 1)
            ret << my_values[i]->representation() << "," << endl;
        else 
            ret << my_values[i]->representation() << endl;
    }
    
    for (int j = 0; j < get_depth(); ++j)
        ret << "\t";
    ret << "]";
    return ret.str();
}

JString::JString()
    : JValue(STRING) , my_value("")
{
}

JString::JString(const string& value)
    : JValue(STRING), my_value(value)
{
}

JString& JString::operator=(const string& str)
{
    my_value = str;
    return *this;
}

const std::string& JString::get_value() const
{
    return my_value;
}

string JString::representation() const
{
    return "\"" + my_value + "\"";
}

JNumber::JNumber(double value)
    : JValue(NUMBER), my_value(value)
{
}

JNumber& JNumber::operator=(double value)
{
    my_value = value;
    return *this;
}

double JNumber::get_value()const
{
    return my_value;
}

string JNumber::representation() const
{
    ostringstream ret;
    ret << my_value;
    return ret.str();
}

JBool::JBool()
    :JValue(BOOL), my_value(false)
{
}

JBool::JBool(bool val)
    :JValue(BOOL), my_value(val)
{
}

JBool& JBool::operator=(bool val)
{
    my_value = val;
    return *this;
}

string JBool::representation() const
{
    if (my_value)
        return "true";
    else
        return "false";
}

JNull::JNull()
    :JValue(NULL_TYPE)
{
}

string JNull::representation() const
{
    return "null";
}

void JParser::input(const std::string& input) {
    for (auto& c : input)
        my_input.push_back(c);
}

void j_val_destroy(j_val* val) {
    JPtr* ptr = (JPtr*) val;
    delete ptr;
}

char* j_val_representation(j_val* val) {
    JPtr* ptr = (JPtr*) val;
    string rep = (*ptr)->representation();
    return strdup(rep.c_str());
}

jtype j_val_get_type(j_val* val) {
    JPtr* ptr = (JPtr*)val;
    return (*ptr)->get_type();
}

j_object* j_object_create() {
    j_object* ptr = NULL;
    try {
        JObjectPtr* smartptr = new std::shared_ptr<JObject> (new JObject);
        ptr = (j_object*) smartptr;
    } catch (...) {
        /*don't allow exceptions to reach C*/
    }
    return ptr;
}

void j_object_destroy(j_object* obj) {
    JObjectPtr* ptr = (JObjectPtr*) obj;
    delete ptr;
}

void j_object_add_value(j_object* obj, const char* key, j_val* val)
{
    JObjectPtr* objptr = (JObjectPtr*) obj;
    JPtr* valptr = (JPtr*) val;
    (*objptr)->add_value(key, *valptr);
}

j_val* j_object_get_value(j_object* obj, const char* key)
{
    j_val* ret = NULL;
    try {
        JObjectPtr* objptr = (JObjectPtr*) obj;
        JPtr* retptr = new std::shared_ptr<JValue>((*objptr)->get_value(key));
        ret = (j_val*) retptr;
    } catch (...) {
    }
    return ret;
}

j_array* j_array_create() {
    j_array* ret = NULL;
    try{
        JArrayPtr* ptr = new std::shared_ptr<JArray>(new JArray);
        ret = (j_array*) ptr;
    } catch (...) {
    }
    return ret;
}

void j_array_destroy(j_array* array) {
    JArrayPtr* ptr = (JArrayPtr*) array;
    delete ptr;
}

void j_array_add_value(j_array* arr, j_val* val) {
    JArrayPtr* arrptr = (JArrayPtr*) arr;
    JPtr* valptr = (JPtr*) val;
    (*arrptr)->add_value(*valptr);
}

j_val* j_array_get_value(j_array* arr, unsigned i) {
    j_val* ret = NULL;
    try {
        JArrayPtr* arrptr = (JArrayPtr*) arr;
        JPtr* retptr = new std::shared_ptr<JValue>((*arrptr)->get_value(i));
        ret = (j_val*) retptr;
    } catch(...) {
    }
    return ret;
}

unsigned j_array_size(j_array* arr) {
    JArrayPtr* ptr = (JArrayPtr*) arr;
    return (*ptr)->size();
}


j_string* j_string_create(){
    j_string* ret = NULL;
    try {
        JStringPtr* retptr;
        retptr = new std::shared_ptr<JString>(new JString);
        ret = (j_string*) retptr;
    } catch (...) {
    }
    return ret;
}

j_string* j_string_create_str(const char* str) {
    j_string* ret = NULL;
    try {
        JStringPtr* strptr = new std::shared_ptr<JString>(new JString(str));
        ret = (j_string*) strptr;
    } catch(...) {
    }
    return ret;
}

void j_string_destroy(j_string* str) {
    JStringPtr* ptr = (JStringPtr*) str;
    delete ptr;
}

j_string* j_string_assign(j_string* str, const char* value)
{
    try{
        JStringPtr* strptr = (JStringPtr*) str;
        **strptr = value;
    } catch (...) {
    }
    return str;
}

j_number* j_number_create(double number)
{
    j_number* ret = NULL;
    try {
        JNumberPtr* ptr = new std::shared_ptr<JNumber>(new JNumber(number));
        ret = (j_number*) ptr;
    } catch(...){
    }
    return ret;
}

const char* j_string_get_value(j_string* str) {
    const char* ret = NULL;
    try {
        JStringPtr* ptr = (JStringPtr*) str;
        const string& s = (*ptr)->get_value();
        ret = s.c_str();
    } catch (...) {
    }
    return ret;
}

void j_number_destroy(j_number* num) {
    JNumberPtr* ptr = (JNumberPtr*) num;
    delete ptr;
}

j_number* j_number_assign(j_number* num, double value)
{
    j_number* ret = NULL;
    try {
        JNumberPtr* ptr = (JNumberPtr*) num;
        **ptr = value;
        ret = (j_number*) ptr;
    } catch (...) {
    }
    return ret;
}

double j_number_get_value(j_number* num)
{
    JNumberPtr* ptr = (JNumberPtr*) num;
    return (*ptr)->get_value();
}

j_bool* j_bool_create()
{
    j_bool* ret = NULL;
    try {
        JBoolPtr* ptr = new std::shared_ptr<JBool>(new JBool);
        ret = (j_bool*) ptr;
    } catch(...) {
    }
    return ret;
}

void j_bool_destroy(j_bool* b) {
    JBoolPtr* ptr = (JBoolPtr*) b;
    delete ptr;
}

j_bool* j_bool_create_bool(int b)
{
    j_bool* ret = NULL;
    try {
        JBoolPtr* ptr = new std::shared_ptr<JBool>(new JBool(b != 0));
        ret = (j_bool*) ptr;
    } catch(...) {
    }
    return ret;
}

j_bool* j_bool_assign(j_bool* b, int value)
{
    j_bool* ret = NULL;
    try {
        JBoolPtr* ptr = (JBoolPtr*) b;
        **ptr = value != 0;
    } catch(...) {
    }
    return ret;
}

j_null* j_null_create() {
    j_null* ret = NULL;
    try {
        JNullPtr* ptr = new std::shared_ptr<JNull>(new JNull);
        ret = (j_null*) ptr;
    }
    catch(...) {
    }
    return ret;
}

void j_null_destroy(j_null* n) {
    JNullPtr* ptr = (JNullPtr*) n;
    delete ptr;
}
