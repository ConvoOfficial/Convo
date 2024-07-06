#pragma once

/*────────────────────────────────────────────────────────────────────────────*/

#include "html.cpp"
#include "db.cpp"

/*────────────────────────────────────────────────────────────────────────────*/

namespace convo {

    express_tcp_t contact(){

        express_tcp_t app;
    
    /*─······································································─*/

        app.GET("/rmov/:client",[]( express_http_t cli ){
            
            db::remove_contact( cli.params["user"],
                url::normalize( cli.params["client"] )
            );

            END:; cli.redirect( 
                cli.headers["Referer"].empty() ? 
                 "/convo" : cli.headers["Referer"]
            );

        });

    /*─······································································─*/

        app.ALL([]( express_http_t cli ){

            auto file = fs::readable( "./View/components/contact.html" );
            auto data = stream::await( file );
            auto user = cli.params["user"];

            string_t filter;

            if( cli.method == "POST" && !cli.headers["Content-Length"].empty() ){
                auto len = string::to_ulong( cli.headers["Content-Length"] );
                filter   = url::normalize( cli.read( len ).slice( 5, 15 ) );
            }

            cli.render( html::render( data, header_t({
                { "<!--BADGE-->"  , html::badge_notification(cli.params["user"]) },
                { "<!--CONTACT-->", html::get_all_users( user, filter ) },
                { "<!--USERS-->"  , html::get_contacts( user ) },
            }) ));

        });

    /*─······································································─*/

        return app;

    }

}

/*────────────────────────────────────────────────────────────────────────────*/