#pragma once
#include <nodepp/crypto.h>
#include <nodepp/map.h>
#include <nodepp/fs.h>

/*────────────────────────────────────────────────────────────────────────────*/

#include "db.cpp"

/*────────────────────────────────────────────────────────────────────────────*/

namespace convo { namespace html {

    string_t notification( string_t message, string_t color ) {
        return regex::format( R"(
            <p style="background: ${0}; color: white; padding: 10px; margin: 0px;" 
               class="uk-animation-shake" > ${1} </p>
        )", color, message );
    }

    /*.........................................................................*/

    string_t badge_notification( string_t user ){
        auto data = db::get_state( user ); ulong count=0;

        for( auto &x: data["cont"].as<array_t<object_t>>() ){
             count += x["notf"].as<int>();
        }   

        if( count >= 10 ){
            return R"(
            <a button class="uk-inline" href="/convo/notification" >
                <i badge class="uk-position-top-right"> +9 </i>
                <i material> notifications </i>
            </a>)";
        } elif( count != 0 ){
            return string::format( R"(
            <a button class="uk-inline" href="/convo/notification" >
                <i badge class="uk-position-top-right"> %lu </i>
                <i material> notifications </i>
            </a>)", count );
        } else {
            return R"(
            <a button class="uk-inline" href="/convo/notification" >
                <i material> notifications </i>
            </a>)";
        }

    }

    /*.........................................................................*/

    string_t new_message( bool self, string_t name, string_t msg ) {
        string_t data = regex::format(R"(
            <div class="uk-inline uk-width-1-3@m uk-width-2-3 
                        uk-animation-slide-bottom-small
                        uk-animation-fade" 
                 ${2} message> 
                <badge> ${0} </badge> ${1} 
            </div>
        )",name,regex::replace_all(msg,"[><]",""),self?"right":"left");
        return encoder::hex::get(data.size()) + "\r\n" + data + "\r\n";
    }

    /*.........................................................................*/

    string_t& render( string_t& data ){
        while( regex::test( data, "<°[^°]+°>" ) ){
            auto pttr = regex::match( data, "<°[^°]+°>" );
            auto name = regex::match( pttr, "[^<°> \n\t]+" );

            if( fs::exists_file( name ) ){ 
                auto str = stream::await( fs::readable( name ) );
                data = regex::replace_all( data, pttr, str );
            } else {
                data = regex::replace_all( data, pttr, "file does not exists" );
            }
            
        }   return regex::test(data,"<°[^°]+°>") ? render(data) : data;
    }

    /*.........................................................................*/

    string_t& render( string_t& data, map_t<string_t,string_t> list ){
        data = render( data ); for( auto &x: list.data() ){
        data = regex::replace_all( data,x.first,x.second );
        }      return data;
    }

    /*.........................................................................*/

    string_t get_contacts( string_t user ){

        auto list = db::get_all_contacts( user ); string_t res;

        res += R"(<ul class="uk-list uk-list-divider uk-dark-divider uk-margin-remove">)";

        for( ulong x=0; x < list.size(); x++ ){ 
             auto y = list[x]["name"].as<string_t>();
             auto z = list[x]["notf"].as<int>();
             auto n = string_t();

        if ( z>=10 ){ n = R"(<i badge> +9 </i>)"; }
        elif( z!=0 ){ n = string::format(R"(<i badge> %u </i>)",z); }

            res += regex::format( R"(
                <li class="uk-flex uk-flex-between uk-padding-small"> 
                <div class="uk-flex uk-flex-middle">
                    <a href="/convo/contact/rmov/${0}" material> close </a>
                    <a href="/convo/${0}" link > ${0} </a>
                </div><div> ${2} ${1} </div>
                </li>
            )", y, db::is_connected(y) ? "🟢" : "🔴", n );
        }

        res += R"(</ul>)"; return res;
    }

    /*.........................................................................*/

    string_t get_all_users( string_t user, string_t filter ){

        auto list = db::get_all_users(); string_t res;

        res += R"(<ul class="uk-list uk-list-divider uk-dark-divider uk-margin-remove">)";

        for( ulong x=0; x < list.size(); x++ ){ auto y = list[x];
        if ( y == user || ( !regex::test( y,filter ) && !filter.empty() ) )
           { continue; }

            res += regex::format( R"(
                <li class="uk-flex uk-flex-between uk-padding-small"> 
                <div class="uk-flex uk-flex-middle">
                    <a href="/convo/contact/rmov/${0}" material hidden> close </a>
                    <a href="/convo/${0}" link > ${0} </a>
                </div><div> ${1} </div>
                </li>
            )", y, db::is_connected(y) ? "🟢" : "🔴" );
        }

        res += R"(</ul>)"; return res;
    }

}}