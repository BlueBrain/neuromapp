namespace impl {

    template < typename T >
    void deleter(void * p) {
        delete (T*)p;
    };

}

template < typename T >
T & storage::put_copy(std::string const & name, const T &x) {

    std::map < std::string, impl::container >::iterator it = M.find(name);

    if(it != M.end() ) // if the data already exists, make sure to delete it correctly before I overwrite everything with map.insert
       impl::deleter<T>( it->second.data_ );

    T * c = new T(x);

    M.insert( std::make_pair(name, impl::container(c, &impl::deleter<T> ) ) );

    return *c;
}

template < typename T, class F>
T & storage::get(std::string const & name, F f) {

    std::map < std::string, impl::container >::iterator it = M.find(name);

    if ( it != M.end() ) {
        if ( *(it->second.tid_) != typeid(T) ) {
            throw bad_type_exception(std::string("requested data of name") + name + "already exists but is of different type");
        }
        return *(T*)(it->second.data_);
    }
    else {
        return put_copy<T> ( name, f() );
    }
}

template < typename T>
T & storage::get(std::string const & name) {

    std::map < std::string, impl::container >::iterator it = M.find(name);

    if ( it != M.end() ) {
        if ( *(it->second.tid_) != typeid(T) ) {
            throw bad_type_exception(std::string("requested data of name ") + name + " already exists but is of different type");
        }
        return *(T*)(it->second.data_);
    }
    else {
        throw bad_type_exception(std::string("requested data of name ") + name + " is not present in the storage and no constructor has been provided");
    }
}
