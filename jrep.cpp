
#define INSIDE_LEXER //include the interface to the buffer management
#define INSIDE_PARSER//include the interface to yyparse
#include "jrep.h"
#undef INSIDE_PARSER
#undef INSIDE_LEXER 

#include "jrep-config.h"

#include <sstream>
#include <cstring>
#include <memory>

using std::ostringstream;
using std::string;
using std::map;
using std::deque;
using std::vector;
using std::endl;

extern "C" {
    int yyparse(j_val** parse_result);
    void lexer_destroy_buffer();
}

string jtype_to_string(jtype t)
{
    switch(t){
        case JOBJECT:   return "JObject";
        case JARRAY:    return "JArray";
        case JSTRING:   return "JString";
        case JNUMBER:   return "JNumber";
        case JBOOL:     return "JBool";
        case JNULL:     return "JNull";
        default:        return ""; //Hmm Json extension to binary tree eeg.
    }
}

JCastException::JCastException(jtype is, jtype to)
    : my_to(to), my_is(is)
{
    my_msg = "Unable to cast \"" + jtype_to_string(is) + "\" to \"" +
        jtype_to_string(to) + "\"";
}

JCastException::~JCastException() noexcept
{
}

const char* JCastException::what() const noexcept
{
    return my_msg.c_str();
}

JValue::JValue(jtype t)
    : my_type(t), my_depth(0)
{
}

JValue::~JValue()
{
}

void JValue::set_depth(int depth)
{
    my_depth = depth;
}

void JValue::fix_depth(int depth)
{
    set_depth(depth);
}

JObject& JValue::get_object() throw(JCastException)
{
    jtype t;
    if ((t = get_type()) != JOBJECT)
        throw JCastException(t, JOBJECT);
    return dynamic_cast<JObject&>(*this);
}

JArray& JValue::get_array() throw(JCastException)
{
    jtype t;
    JArrayPtr ret;
    if ((t = get_type()) != JARRAY)
        throw JCastException(t, JARRAY);
    return dynamic_cast<JArray&>(*this);
}

JString& JValue::get_string() throw(JCastException)
{
    jtype t;
    JStringPtr ret;
    if ((t = get_type()) != JSTRING)
        throw JCastException(t, JSTRING);
    return dynamic_cast<JString&>(*this);
}

JNumber& JValue::get_number() throw(JCastException)
{
    jtype t;
    if ((t = get_type()) != JNUMBER)
        throw JCastException(t, JNUMBER);
    
    return dynamic_cast<JNumber&>(*this);
}

JBool& JValue::get_bool() throw(JCastException)
{
    jtype t;
    JNumberPtr ret;
    if ((t = get_type()) != JBOOL)
        throw JCastException(t, JBOOL);
    return dynamic_cast<JBool&>(*this);
}

JNull& JValue::get_null() throw (JCastException)
{
    jtype t;
    if ((t = get_type()) != JNULL)
        throw JCastException(t, JNULL);
    return dynamic_cast<JNull&>(*this);
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
    JValue(JOBJECT)
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

void JObject::fix_depth(int depth) {
    JValue::fix_depth(depth);
    for (auto& pair : my_pairs)
        pair.second->fix_depth((depth + 1));
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
    : JValue(JARRAY)
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

void JArray::fix_depth(int depth)
{
    JValue::fix_depth(depth);
    for (auto& value : my_values)
        value->fix_depth(depth +1);
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
    : JValue(JSTRING) , my_value("")
{
}

JString::JString(const string& value)
    : JValue(JSTRING), my_value(value)
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
    : JValue(JNUMBER), my_value(value)
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
    :JValue(JBOOL), my_value(false)
{
}

JBool::JBool(bool val)
    :JValue(JBOOL), my_value(val)
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

bool JBool::get_value()const
{
    return my_value;
}

JNull::JNull()
    :JValue(JNULL)
{
}

string JNull::representation() const
{
    return "null";
}

void JParser::scan_string(const std::string& input) {
    lexer_scan_string(input.c_str());
}

void JParser::scan_bytes(const std::string& input, int length)
{
    lexer_scan_bytes(input.c_str(), length);
}

void JParser::scan_buffer(char* buffer, size_t size)
{
    lexer_scan_buffer(buffer, size);
}


int JParser::parse(JPtr& output)
{
    j_val* out = NULL;
    int retval = yyparse(&out);
    if (retval == 0){
        JPtr* temp = (JPtr*) out;
        output = *temp;
        j_val_destroy(out);
        //delete temp;
    }
    lexer_destroy_buffer();
    return retval;
}

/************ Implementation of the C API **********/

const char* jrep_version() {
    return JREP_VERSION_STRING;
}

unsigned jrep_major_version() {
    return JREP_VERSION_MAJOR;
}

unsigned jrep_minot_version() {
    return JREP_VERSION_MINOR;
}

unsigned jrep_micro_version() {
    return JREP_VERSION_MICRO;
}

void j_val_destroy(j_val* val) {
    jtype t = j_val_get_type(val);
    switch(t) {
        case JSTRING:
            j_string_destroy((j_string*) val);
            break;
        case JNUMBER:
            j_number_destroy((j_number*) val);
            break;
        case JOBJECT:
            j_object_destroy((j_object*) val);
            break;
        case JARRAY:
            j_array_destroy((j_array*) val);
            break;
        case JBOOL:
            j_bool_destroy((j_bool*) val);
            break;
        case JNULL:
            j_null_destroy((j_null*) val);
            break;
    }
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

void j_val_fix_depth(j_val* val, int depth){
    JPtr* ptr = (JPtr*) val;
    (*ptr)->fix_depth(depth);
}

size_t j_array_size(j_array* arr) {
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

int j_bool_get_value(j_bool* b)
{
    JBoolPtr* ptr = (JBoolPtr*) b;
    return (*ptr)->get_value();
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
