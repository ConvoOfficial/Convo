#pragma once
#include <nodepp/object.h>
#include <nodepp/cookie.h>
#include <nodepp/query.h>
#include <nodepp/path.h>
#include <nodepp/fs.h>
#include <jwt/jwt.h>

/*────────────────────────────────────────────────────────────────────────────*/

#include "html.cpp"
#include "db.cpp"

/*────────────────────────────────────────────────────────────────────────────*/

#define INVALID "[\"\'\n\t$*% ]"

/*────────────────────────────────────────────────────────────────────────────*/

namespace convo {

    express_tcp_t image() {

        express_tcp_t app;

        app.GET("/empty",[]( express_http_t cli ){
            cli.header( "Cache-Control", "public, max-age=86400" );
            auto list = fs::read_folder( path::join( "View","img","empty" ) );
            static uchar x = 0; x++; x %= list.size();
            cli.sendFile( path::join( "View","img","empty",list[x] ) );
        });

        app.GET("/convo",[]( express_http_t cli ){
            cli.header( "Cache-Control", "public, max-age=86400" );
            auto list = fs::read_folder( path::join( "View","img","chat" ) );
            static uchar x = 0; x++; x %= list.size();
            cli.sendFile( path::join( "View","img","chat",list[x] ) );
        });

        app.ALL([]( express_http_t cli ){
            cli.status(404).send("Oops file not found");
        });

        return app;

    }
    
    /*─······································································─*/

    express_tcp_t signin() {

        auto app = express_tcp_t();

        app.POST([]( express_http_t cli ){

            auto raw  = regex::replace_all(cli.read(),"[\n\r]+","&");
            auto data = query::parse( "?" + raw.slice( 0, -1 ) );
            cli.header( "Content-Type", "text/html" );

            auto cb = []( string_t message ){
                auto dir  = path::join( "View", "signin.html" );
                auto file = stream::await( fs::readable( dir ));
                return convo::html::render( file, header_t({
                    { "<!--MESSAGE-->", html::notification( message, "red" ) }
                }));
            };

            if( data["user"].empty() ){
                cli.send( cb( "username is empty" ) ); return;
            }   data["user"] = regex::replace_all( data["user"], INVALID, "" );

            if( data["pass"].empty() ){
                cli.send( cb( "password is empty" ) ); return;
            }   data["pass"] = regex::replace_all( data["pass"], INVALID, "" );

            if( !db::check_user( data["user"], data["pass"] ) ){
                cli.send( cb( "invalid password or username" ) ); return;
            }

            cli.header( "Set-Cookie", cookie::format( cookie_t({

                { "auth", jwt::HS256::encode( json::stringify( 
                    object_t({ { "user", data["user"] } })
                ), process::env::get("SECRET")) },

                { "max-age", string::to_string( 
                    string::to_int(data["time"]) * 3600 
                )}

            })));

            cli.redirect("/"); // successful

        });

        app.ALL([]( express_http_t cli ){ 

            if(!cli.headers["Cookie"].empty() ){
                auto data = cookie::parse( cli.headers["Cookie"] );
            if( db::verify_auth( data["auth"] ) ){ 
                cli.redirect("/convo");
            }}

        }); 
        
        return app;

    }
    
    /*─······································································─*/

    express_tcp_t signup() {

        auto app = express_tcp_t();

        app.POST([]( express_http_t cli ){

            auto raw  = regex::replace_all(cli.read(),"[\n\r]+","&");
            auto data = query::parse( "?" + raw.slice( 0, -1 ) );
            cli.header( "Content-Type", "text/html" );

            auto cb   = []( string_t message ){
                auto dir  = path::join( "View", "signup.html" );
                auto file = stream::await( fs::readable( dir ));
                return convo::html::render( file, header_t({
                    { "<!--MESSAGE-->", html::notification( message, "red" ) }
                }));
            };

            if( data["user"].size() > 10 ){
                cli.send( cb( "username exceeds the limit | 10 chars" ) ); return;
            }

            if( data["pass1"].size() > 64 ){
                cli.send( cb( "passwords exceeds the limit | 64 chars" ) ); return;
            }

            if( data["pass1"] != data["pass2"] ){
                cli.send( cb( "passwords does not match" ) ); return;
            }

            if( data["user"].empty() || regex::test( data["user"], INVALID ) ){
                cli.send( cb( "username contains invalid chars" ) ); return;
            }

            if( data["pass1"].empty() || regex::test( data["pass"], INVALID ) ){
                cli.send( cb( "password contains invalid chars" ) ); return;
            }

            if( db::user_exists( data["user"] ) ){
                cli.send( cb( "user already exist, try with other name" ) ); return;
            }

            if( !db::add_user( data["user"], data["pass1"] ) ){
                cli.send( cb( "something went wrong, try later" ) ); return;
            }

            cli.redirect("/signin"); // successful

        });

        app.ALL([]( express_http_t cli ){ 

            if(!cli.headers["Cookie"].empty() ){
                auto data = cookie::parse( cli.headers["Cookie"] );
            if( db::verify_auth( data["auth"] ) ){ 
                cli.redirect("/convo");
            }}

        }); 
        
        return app;

    }
    
    /*─······································································─*/

    express_tcp_t login() {

        auto app = express_tcp_t();

        app.ALL([]( express_http_t cli ){ 

            if(!cli.headers["Cookie"].empty() && cli.path == "/" ){
                auto data = cookie::parse( cli.headers["Cookie"] );
            if( db::verify_auth( data["auth"] ) ){ 
                cli.redirect("/convo");
            }}

        }); 
        
        return app;

    }
    
    /*─······································································─*/

    express_tcp_t logout() {

        auto app = express_tcp_t();

        app.ALL([]( express_http_t cli ){ 
            cli.clear_cookies(); 
            cli.redirect("/signin"); 
        });
        
        return app;

    }

}

/*────────────────────────────────────────────────────────────────────────────*/

#undef INVALID