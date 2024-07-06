#pragma once 
#include <nodepp/path.h>
#include <nodepp/url.h>
#include <jwt/jwt.h>

/*────────────────────────────────────────────────────────────────────────────*/

#include "message.cpp"
#include "contact.cpp"
#include "db.cpp"

/*────────────────────────────────────────────────────────────────────────────*/

namespace convo {

    express_tcp_t chat() {

        express_tcp_t app;
    
    /*─······································································─*/

        app.ALL([]( express_http_t cli ){

            if( !cli.headers["Cookie"].empty() ){
                
                auto data = cookie::parse( cli.headers["Cookie"] );
                if( !db::verify_auth( data["auth"] ) ){ goto END; }
                
                auto user = json::parse( jwt::HS256::decode( data["auth"] ) )["user"];

                if( !user.has_value() )                      { goto END; }
                if( !db::user_exists( user.as<string_t>() ) ){ goto END; }

                cli.params["user"] = user.as<string_t>();

            } else { END:;
                if ( cli.headers["Sec-Fetch-Dest"] == "iframe" )
                   { cli.redirect("/components/disconnected"); }
              else { cli.redirect("/signin"); }
            }

        });
    
    /*─······································································─*/

        app.USE( "/message", message() );
        app.USE( "/contact", contact() );
    
    /*─······································································─*/

        app.GET( "/notification", []( express_http_t cli ){
            
            auto file = fs::readable( path::join("View","components","notification.html") );
            auto data = stream::await( file );

            cli.render( html::render( data, header_t({
                { "<!--BADGE-->"  , html::badge_notification(cli.params["user"]) },
                { "<!--CONTACT-->", html::get_contacts(cli.params["user"]) },
                { "<!--CLIENT-->" , url::normalize(cli.params["client"]) },
                { "<!--PREV-->"   , cli.headers["Referer"] },
                { "<!--USNAME-->" , cli.params["user"] },
            }) ));
            
        });
    
    /*─······································································─*/

        app.USE( "/:client", []( express_http_t cli, function_t<void> next ){ 
            if( cli.params["client"].empty() ){ next(); return; }

            db::append_contact( cli.params["user"], 
                url::normalize( cli.params["client"] )
            );

            db::remove_notification( cli.params["user"], 
                url::normalize( cli.params["client"] )
            );

            next();

        });
    
    /*─······································································─*/

        app.GET([]( express_http_t cli ){

            auto file = fs::readable( "./View/components/chat.html" );
            auto data = stream::await( file );

            cli.render( html::render( data, header_t({
                { "<!--BADGE-->"  , html::badge_notification(cli.params["user"]) },
                { "<!--CONTACT-->", html::get_contacts(cli.params["user"]) },
                { "<!--CLIENT-->" , url::normalize(cli.params["client"]) },
                { "<!--PREV-->"   , cli.headers["Referer"] },
                { "<!--USNAME-->" , cli.params["user"] },
            }) ));

        });
    
    /*─······································································─*/

        return app;

    }
    
}

/*────────────────────────────────────────────────────────────────────────────*/