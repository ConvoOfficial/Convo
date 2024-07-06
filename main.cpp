#include <nodepp/nodepp.h>
#include <express/http.h>
#include <nodepp/date.h>

using namespace nodepp;

#include "Controller/contact.cpp"
#include "Controller/login.cpp"
#include "Controller/chat.cpp"
#include "Controller/db.cpp"

void onMain() {

    process::env::init( ".env" );
    auto app = express::http::add();

    /*··················································*/

    convo::db::init();

    /*··················································*/

    app.USE([]( express_http_t cli, function_t<void> next ){
        if( cli.headers["User-Agent"].empty() )
          { cli.send(); return; } next();
    });

    /*··················································*/

    app.USE( "/signin" , convo::signin() );
    app.USE( "/signup" , convo::signup() );
    app.USE( "/logout" , convo::logout() );

    /*··················································*/

    app.USE(             convo::login()  );
    app.USE( "/convo"  , convo::chat()   );

    /*··················································*/
    
    app.USE( "/image"  , convo::image() );
    app.USE( express::http::ssr("View") );

    /*··················································*/

    app.listen( "0.0.0.0", 3000, []( ... ){
        console::log( "http://localhost:3000" );
    });

}