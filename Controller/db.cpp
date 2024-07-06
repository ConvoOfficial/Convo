#pragma once
#include <nodepp/crypto.h>
#include <nodepp/object.h>
#include <nodepp/json.h>
#include <redis/http.h>
#include <jwt/jwt.h>

/*────────────────────────────────────────────────────────────────────────────*/

namespace convo { namespace db {

    string_t get_chat_id( string_t user, string_t cli ){

        auto sha1 = crypto::hash::SHA1(),
             sha2 = crypto::hash::SHA1(),
             sha3 = crypto::hash::SHA1();

        sha1.update( url::normalize(user) ); sha2.update( url::normalize(cli) );
        sha3.update(( sha1.get() + sha2.get() ).sort([]( char a, char b ){ return a<=b; }));

        return sha3.get();
    }

    /*.........................................................................*/
    
    bool check_user( string_t user, string_t pass ){

        auto cli = redis::http::add(process::env::get("REDIS"));
        auto hah = crypto::hash::SHA1(); hah.update( user+pass );
        auto dta = cli.raw( string::format("HMGET convo-user '%s'", user.get() ) );

        return regex::test( dta, hah.get(), true );
    }
    
    /*─······································································─*/

    bool user_exists( string_t user ) {

        auto cli = redis::http::add(process::env::get("REDIS"));
        auto dta = cli.raw( string::format("HMGET convo-user '%s'", user.get() ) );

        return !regex::test( dta, "\\$-1", true );
    }
    
    /*─······································································─*/

    array_t<string_t> get_all_users() {
        auto cli = redis::http::add(process::env::get("REDIS"));
        return cli.exec( "HKEYS convo-user" );
    }
    
    /*─······································································─*/

    bool verify_auth( string_t token ){
        if( !jwt::HS256::verify( token, process::env::get("SECRET") ) ){ return false; }
        
        auto data = json::parse( jwt::HS256::decode(token) );
        if( !data["user"].has_value() ){ return false; }
        
        return user_exists( data["user"].as<string_t>() );
    }

    /*─······································································─*/

    object_t get_state( string_t user ) {
        if( !user_exists(user) ){ return nullptr; }

        auto cli = redis::http::add(process::env::get("REDIS"));
        auto dta = cli.exec( string::format("HMGET convo-user '%s'", user.get() ) );

        if ( dta.empty() ){ return nullptr; } return json::parse( dta[0] );
    }

    bool set_state( string_t user, object_t value ) {
        if( !value.has_value() ){ return false; }
        if(  user.empty() )     { return false; }

        auto cli = redis::http::add(process::env::get("REDIS"));
             
        cli.raw( string::format("HMSET convo-user '%s' '%s'", 
            user.get(), json::stringify( value ).get()
        ));

        return true;
    }

    /*─······································································─*/

    void append_notification( string_t from, string_t to ){
         if( !user_exists( from ) ){ return; }
         if( !user_exists( to ) )  { return; }
         if( from == to )          { return; }

        auto state = get_state( to );
        auto cont  = state["cont"].as<array_t<object_t>>();

        for( ulong x=0; x<cont.size(); x++ ){
        if ( cont[x]["name"].as<string_t>() == from ){
        if ( cont[x]["notf"].as<uchar>() > 10 ){ return; }
             cont[x]["notf"] = cont[x]["notf"].as<uchar>()+1;
             state["cont"] = cont; set_state( to, state ); return;
        }}

        cont.push( object_t({ { "notf", (uchar) 1 }, { "name", from } }) ); 
        state["cont"] = cont; set_state( to, state );
    }

    void remove_notification( string_t from, string_t to ){
         if( !user_exists( from ) ){ return; }
         if( !user_exists( to ) )  { return; }
         if( from == to )          { return; }

        auto state = get_state( from );
        auto cont  = state["cont"].as<array_t<object_t>>();

        for( ulong x=0; x<cont.size(); x++ ){
        if ( cont[x]["name"].as<string_t>() == to ){ 
             cont[x]["notf"] = (uchar) 0;
             set_state( from, state );
             return;
        }}

    }

    /*─······································································─*/

    void append_message( string_t from, string_t to, string_t id, string_t msg ){
        if( !user_exists(from) ){ return; }
        if( !user_exists(to) )  { return; }
        if( from == to )        { return; }

        auto data = object_t ({
            { "from", from }, { "to", to },
            { "msg",  encoder::base64::get( msg )  }
        });

        auto cli = redis::http::add(process::env::get("REDIS"));
        cli.exec( string::format("RPUSH convo-%s '%s'", id.get(), 
            encoder::base64::get(json::stringify( data )).get()
        ));
         
    }

    void get_messages( string_t id, function_t<void,string_t> cb ){
        auto cli = redis::http::add(process::env::get("REDIS"));
        cli.exec( string::format("LRANGE convo-%s 0 -1", id.get() ), cb );
    }

    /*─······································································─*/

    void append_contact( string_t user, string_t contact ){
        if( !user_exists( contact ) ){ return; }
        if( !user_exists( user ) ){ return; }
        if( user == contact ){ return; }

        auto state = get_state( user );
        auto cont  = state["cont"].as<array_t<object_t>>();

        for( ulong x=0; x<cont.size(); x++ ){
        if ( cont[x]["name"].as<string_t>() == contact ){ return; }
        }

        cont.push( object_t({
            { "name", contact  },
            { "notf", (uchar)0 }
        }) );
        
        state["cont"] = cont; set_state( user, state );
    }

    void remove_contact( string_t user, string_t contact ){
        if( !user_exists( contact ) ){ return; }
        if( !user_exists( user ) ){ return; }

        auto state = get_state( user );
        auto cont  = state["cont"].as<array_t<object_t>>();

        for( ulong x=0; x<cont.size(); x++ ){
        if ( cont[x]["name"].as<string_t>() == contact ){ 
             cont.erase( x );
        }}

        state["cont"] = cont; set_state( user, state );
    }

    /*─······································································─*/

    bool is_connected( string_t user ){
        if( !user_exists( user ) ){ return false; }
        auto   data = get_state( user );
        return data["stat"].as<uchar>()>0;
    }

    void reset_connection( string_t user ){
        if( !user_exists( user ) ){ return; }
        auto data = get_state( user );
             data["stat"] = (uchar) 0;
        set_state( user, data );
    }

    void set_disconnected( string_t user ){
        if( !user_exists( user ) ){ return; }
        auto data = get_state( user );
        if ( data["stat"].as<uchar>() == 0 ){ return; }
             data["stat"] = data["stat"].as<uchar>()-1;
        set_state( user, data );
    }

    void set_connected( string_t user ){
        if( !user_exists( user ) ){ return; }
        auto data = get_state( user );
        if ( data["stat"].as<uchar>() == 255 ){ return; }
             data["stat"] = data["stat"].as<uchar>()+1;
        set_state( user, data );
    }

    /*─······································································─*/
    
    bool add_user( string_t user, string_t pass ){

        auto hah = crypto::hash::SHA1(); hah.update( user+pass );

        return set_state( user, object_t({
            { "stat", (uchar) 0  },
            { "pass", hah.get()  },
            { "chat", string_t() },
            { "last", string_t() },
            { "cont", array_t<object_t>() }
        }));

    }
    
    /*─······································································─*/

    object_t get_all_contacts( string_t user ) {
        if( !user_exists( user ) ){ return nullptr; }
        return get_state( user )["cont"];
    }

    /*.........................................................................*/

    void init(){
        for( auto &x: get_all_users() ){
             reset_connection( x );
        }
    }

}}

/*────────────────────────────────────────────────────────────────────────────*/