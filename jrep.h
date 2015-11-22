
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
        
        /**
         * Get the represtation of the json value of this value.
         *
         * @return the string that is the representation of this object.
         */
        virtual std::string representation() const = 0;

        /**
         * Querry the json type of this value.
         *
         * @return the type of json value.
         */
        jtype get_type()const;

        /**
         * get the depth of this value.
         *
         * @return the depth of this object.
         */
        int get_depth() const;

        /**
         * Sets the depth of this object.
         *
         * @param depth, the new depth of this value
         */

        void set_depth(int depth = 0);

        /**
         * Recursively fix depth setting in this value all possible values below this one
         * also will be fixed.
         */
        virtual void fix_depth(int depth);

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

        /**
         * Add a new key and value to the JObject. If the key already
         * exists, the key is overwritten without questions asked.
         *
         * @param key a new (or existing) key to put a new value on
         * @param value a new json value that is associated with key.
         */
        void add_value(const std::string& key, JPtr value);

        /**
         * Get the value for the key given.
         *
         * @param key a key if key doesn't exist a JObjec::NoSuchKey is thrown
         * @return a smart pointer to JValue
         */
        JPtr get_value(const std::string& key) const;

        /**
         * calls set_depth on *this and calls fix_depth(depth + 1) on
         * all contained values.
         *
         * @param depth the depth of this value.
         */
        void fix_depth(int depth);

    private :

        std::map<std::string, JPtr> my_pairs;

};

typedef std::shared_ptr<JObject> JObjectPtr;

class JArray : public JValue {

    public:

        JArray();

        virtual std::string representation() const;

        /**
         * Adds one value to the contained array. This effectively
         * increases the size with one.
         *
         * @param value A new JValue derived value. Make sure that you
         * don't embedd an array into itself, this would lead to infinite
         * recursion when calling represtation() 
         */
        void add_value(JPtr value);

        /**
         * Obtain the value at the given index.
         *
         * @param index the index within the array must be smaller then size().
         * @return the a smart pointer to JValue contained at index.
         */
        JPtr get_value(std::vector<JPtr>::size_type index)const;

        /**
         * Obtain the size of the array.
         *
         * @return the number of JValue s contained.
         */
        std::vector<JPtr>::size_type size();

        /**
         * Sets the depth of this itemt to depth and all contained values
         * to depth + 1.
         *
         * @param depth the depth where the array is nested inside the JSON
         * representation.
         */
        void fix_depth(int depth);

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

/**
 * \brief Parses jsons string buffers.
 *
 * An instance of JParser can be given string buffers.
 * These buffers will be presented to the lexer, so
 * that the parser will get presented with the input
 * from the supplied buffers.
 */
class JParser {

    public:
        
        /**
         * Copy the string into the lexer.
         *
         * @param input the input string for the lexer.
         */
        void scan_string(const std::string& input);
        
        /**
         * Copy the string into the lexer.
         *
         * @param input the input string for the lexer.
         * @param len the length of the string that
         * should be fed to the lexer
         */
        void scan_bytes(const std::string& input, int len);
        
        /**
         * \brief Give a memory buffer to the lexer
         *
         * Give a buffer to the lexer from all functions this is the
         * most efficient method, since scanning than occurs in place.
         * However, make sure that the buffer ends with two null bytes.
         *
         * @param input the input string for the lexer.
         * @param len the length of the string that
         * should be fed to the lexer
         */
        void scan_buffer(char* buf, size_t size);

        /**
         * \brief parses the input or standard input (stdin).
         * 
         * This method parses the input. Will read from standard input if
         * none of the scan_string, -buffer or -bytes methods has been called.
         *
         * @param JPtr& output JPtr will be initialized with the parsed input.
         * @return 0 = succes, 1 = some kind of syntax error and 2 = out of memory
         */
        int parse(JPtr& output);
};

#endif //__cplusplus

#ifdef __cplusplus
extern "C" {
#endif

#ifdef INSIDE_LEXER
    // These functions will be defined in the third section of lexer.l
    void lexer_scan_string(const char* input);
    void lexer_scan_bytes(const char* bytes, int length);
    void lexer_scan_buffer(char* buffer, size_t length);
#endif

typedef struct j_val j_val;
char*   j_val_representation(j_val* value);
void j_val_destroy(j_val* value);
void j_val_fix_depth(j_val* val, int i);
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
