/*
    The Actual Cache Data Structure.
*/

#include <unordered_map>
#include <string>

namespace CACHE{

    enum class MESSAGE {
        MISS,
        ALREADY_IN,
        GOOD
    };

    struct RVAL{
        std::string value;
        CACHE::MESSAGE message;
    };

    static std::unordered_map< std::string, std::string> cache;

    inline RVAL add(std::string key, std::string value){

        RVAL rval; 
        
        if(!cache.contains(key)){
            cache.insert({key, value});
            rval.message = MESSAGE::GOOD;
        }
        else{
            rval.message = MESSAGE::ALREADY_IN;
            cache[key] = value;
        }

        return rval; 

    }

    inline RVAL update(std::string key, std::string value){
        RVAL rval; 
        if(cache.contains(key)){
            rval.message = MESSAGE::GOOD;
            cache[key] = value;
        }
        else{
            rval.message = MESSAGE::MISS;
        }

        return rval;
        
    }
    
    inline RVAL get(std::string key){
        RVAL rval; 
        if(cache.contains(key)){
            rval.message = MESSAGE::GOOD;
            rval.value = cache[key];
        }
        else{
            rval.message = MESSAGE::MISS;
        }

        return rval;
    }

    inline RVAL del(std::string key){
        RVAL rval;
        if(cache.contains(key)){
            rval.message = MESSAGE::GOOD;
            cache.erase(key);
        }
        else{
            rval.message = MESSAGE::MISS;
        }

        return rval;
    }

}