
#ifndef JREP_H
#define JREP_H

typedef enum jtype {STRING, NUMBER, OBJECT, ARRAY, BOOL, NULL_TYPE}jtype;

#ifdef __cplusplus

#include <deque>
#include <string>
#include <memory>
#include <map>
#include <exception>
#include <vector>

class JValue;
typedef std::shared_ptr<JValue> JPtr;

class JValue{

    public:
        
        virtual std::string representation() const = 0;
        jtype get_type()const;
        int get_depth() const;
        void set_depth(int depth = 0);

    protected:

        JValue(jtype t);
        virtual ~JValue();

    private:

        jtype   my_type;    // Contains the type of json object
        int     my_depth;   // how deep is the object nested

};

class JObject : public JValue {

    public :

        class NoSuchKeyError : public std::exception {
            public:
                NoSuchKeyError(const std::string& key);
                ~NoSuchKeyError()throw();
                const char* what()const throw();
            private:
                std::string my_key;
        };

        JObject();
        
        virtual std::string representation() const;

        void add_value(const std::string& key, JPtr value);

        JPtr get_value(const std::string& key) const;

    private :

        std::map<std::string, JPtr> my_pairs;

};

typedef std::shared_ptr<JObject> JObjectPtr;

class JArray : public JValue {

    public:

        JArray();

        virtual std::string representation() const;

        void add_value(JPtr);

        JPtr get_value(std::vector<JPtr>::size_type index)const;

        std::vector<JPtr>::size_type size();

    private:

        std::vector<JPtr> my_values;
};

typedef std::shared_ptr<JArray> JArrayPtr;

class JString : public JValue {

    public :

        JString();
        JString(const std::string& str);

        virtual std::string representation() const;

        JString& operator= (const std::string& rhs);

        const std::string& get_value()const;

    private:

        std::string my_value;
};

typedef std::shared_ptr<JString> JStringPtr;

class JNumber : public JValue {

    public :

        JNumber(double value);
        
        virtual std::string representation() const;

        JNumber& operator= (double value);
       
        double get_value()const;

    private:
        
        double my_value;
};

typedef std::shared_ptr<JNumber> JNumberPtr;

class JBool : public JValue {
    
    public :

        JBool();
        JBool(bool);
        
        virtual std::string representation() const;

        JBool& operator=(bool value);

    private:

        bool my_value;
};

typedef std::shared_ptr<JBool> JBoolPtr;

class JNull : public JValue {

    public :

        JNull();

        virtual std::string representation() const;

};

typedef std::shared_ptr<JNull> JNullPtr;

class JParser {

    public:
        
        void input(const std::string& input);

        JPtr parse();

    private:

        std::deque<char> my_input;
};

#endif //__cplusplus

#ifdef __cplusplus
extern "C" {
#endif

typedef struct j_val j_val;
char*   j_val_representation(j_val* value);
void j_val_destroy(j_val* value);
jtype   j_val_get_type(j_val* value);

typedef struct j_object j_object;
j_object* j_object_create();
void j_object_destroy(j_object* obj);
void j_object_add_value(j_object* obj, const char* key, j_val* value);
j_val* j_object_get_value(j_object* obj, const char* key);

typedef struct j_array j_array;
j_array* j_array_create();
void j_array_destroy(j_array* array);
void j_array_add_value(j_array* array, j_val* value);
j_val* j_array_get_value(j_array* arr, unsigned index);
size_t j_array_size(j_array* array);

typedef struct j_string j_string;
j_string* j_string_create();
j_string* j_string_create_str(const char* str);
void j_string_destroy(j_string* str);
const char* j_string_get_value(j_string* jstr);
j_string* j_string_assign(j_string* jstr, const char* str);

typedef struct j_number j_number;
j_number* j_number_create(double value);
void j_number_destroy(j_number* num);
j_number* j_number_assign(j_number* num, double value);
double j_number_get_value(j_number* num);

typedef struct j_bool j_bool;
j_bool* j_bool_create();
j_bool* j_bool_create_bool(int val);
void j_bool_destroy(j_bool* b);
j_bool* j_bool_assign(j_bool* b, int value);

typedef struct j_null j_null;
j_null* j_null_create();
void j_null_destroy(j_null* null);

#ifdef __cplusplus
}
#endif

#endif // #ifndef JREP_H
