#pragma once
#include <nodepp/encoder.h>
#include <nodepp/event.h>
#include <nodepp/path.h>
#include <nodepp/url.h>

#include <nodepp/timer.h>

/*────────────────────────────────────────────────────────────────────────────*/

#include "db.cpp"

/*────────────────────────────────────────────────────────────────────────────*/

namespace convo {

    event_t<string_t,string_t, string_t> onMessage;
    event_t<string_t>                    onChange;
    queue_t<express_http_t>              clients;

    express_tcp_t message(){
        
        express_tcp_t app;

    /*.........................................................................*/

        onMessage.on([]( string_t to, string_t from, string_t message ){
            if( !db::is_connected( from ) ){ return; }
            if( !db::user_exists( to ) )   { return; }
            if( message.empty() )          { return; }

            auto dn = false;
            auto id = db::get_chat_id( from, to );
            db::append_message( from, to, id, message );

            auto n = clients.first(); while( n != nullptr ){
            if ( id == n->data.params["ID"] ){ 
            if ( n->data.params["user"] == to ){ dn = true; }
                 auto b = from == n->data.params["user"];
                 n->data.write( html::new_message( b, from, message ));
            }    n = n->next; }

            if( !dn ) db::append_notification( from, to );

        });

    /*.........................................................................*/

        app.ALL("/form/:client",[]( express_http_t cli ){

            if( cli.method == "POST" && !cli.headers["Content-Length"].empty() ){
                auto len = string::to_ulong( cli.headers["Content-Length"] );
                cli.header( "content-type", "text/html" );

                onMessage.emit( 
                    url::normalize(cli.params["client"]),
                    url::normalize(cli.params["user"]),
                    cli.read(len).slice( 4, -2 ) 
                );

            }

            cli.sendFile( "./View/components/message_form.html" );

        });

    /*.........................................................................*/

        app.GET("/:client",[]( express_http_t cli ){
            if(!db::user_exists( url::normalize( cli.params["client"] )) || 
                url::normalize( cli.params["client"] )==cli.params["user"]
            ){ return; }

            cli.params["ID"] = db::get_chat_id( 
                url::normalize(cli.params["client"]),
                url::normalize(cli.params["user"])
            );

            cli.header("Transfer-Encoding", "chunked");
            cli.header("Content-Type", "text/html");
            cli.set_timeout(0); cli.send();
            clients.push( cli ); 

            auto ID = clients.last();

            cli.onDrain.on([=](){
                db::set_disconnected( cli.params["user"] );
                clients.erase(ID);
            }); db::set_connected( cli.params["user"] );

            auto file = fs::readable( "./View/components/message_box.html" );
            auto data = stream::await( file ); 

            data = html::render( data, header_t({
                { "<!--CLIENT-->" , url::normalize(cli.params["client"]) },
            }));

            cli.write( encoder::hex::get(data.size()) + "\r\n" + data + "\r\n" );

            db::get_messages( cli.params["ID"], [=]( string_t message ){
                auto decd = json::parse( encoder::base64::set( message ) );
                auto data = encoder::base64::set(decd["msg"].as<string_t>());
                auto from = decd["from"].as<string_t>();
                auto b    = from==cli.params["user"];
                cli.write( html::new_message( b, from, data ) ); 
            });

            stream::pipe( cli );

        });

    /*.........................................................................*/

        app.GET([]( express_http_t cli ){
            cli.sendFile( "./View/components/empty.html" );
        });

    /*.........................................................................*/

        return app;

    }

}

/*────────────────────────────────────────────────────────────────────────────*/