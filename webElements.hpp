#ifndef WEBELEMENTS_HPP
#define WEBELEMENTS_HPP

#include <ESP8266WebServer.h>
#include <Servo.h> 

struct WebElements
{
    static 
    const String servoSlider(int servo_nr,String handler)
    {
        //value show and submitted on change
        return String()+
        "<form action=\""+handler+"\" method=POST oninput=\"x.value=parseInt(servo_"+servo_nr+".value)\" >"
            "Servo "+servo_nr+": 0<input type=\"range\" name=\"servo_"+servo_nr+"\" min=\"0\" max=\"180\" onchange=\"this.form.submit()\">180 : "
            "<output name=\"x\" for=\"servo_"+servo_nr+"\"></output>"
        "</form>"
        ;
    }

    static 
    const String refreshQuery(String data,int interval)
    {
      const String script_refreshData = String()+
                   "setInterval("
                      "function(){fetch(\"/"+data+"\").then("
                            "function(response){ response.text().then("
                                  "function(text){ document.getElementById(\"showdata_"+data+"\").innerHTML = text; }"
                         ")})}"
                      ","+interval+");";
      return "<p id=\"showdata_"+data+"\">init</p><script>"+script_refreshData+"</script>";
    }

    static 
    const String browserCounter()
    {
      const String script_Counter = 
                       "var a = 0;"
                       "setInterval("
                         "function()"
                           "{a= a+.1; document.getElementById(\"count\").innerHTML =\"web counter: \"+a}"
                         ", 100);";
      return "<p id=\"count\">init</p>"
             "<script>"+script_Counter+"</script>";
    }

    static 
    const String movePad()
    {
        return String() +
        "<table>"
          "<tr>"
            "<th><button type=\"button\" onmousedown=\"fetch('/right')\" onmouseup=\"fetch('/stop')\" style=\"height:25px;width:40px\" ><</button></th>"
            "<th><button type=\"button\" onmousedown=\"fetch('/fwd')\" onmouseup=\"fetch('/stop')\" style=\"height:25px;width:40px\" >^</button></th>"
            "<th><button type=\"button\" onmousedown=\"fetch('/left')\" onmouseup=\"fetch('/stop')\" style=\"height:25px;width:40px\" >></button></th>"
          "</tr>"
          "<tr>"
            "<th><button type=\"button\" onclick=\"fetch('/zero')\" style=\"height:20px;width:20px\" >0</button></th>"
            "<th><button type=\"button\" onmousedown=\"fetch('/back')\" onmouseup=\"fetch('\\stop')\" style=\"height:25px;width:40px\" >v</button></th>"
            "<th><button type=\"button\" onclick=\"fetch('/one')\" style=\"height:20px;width:20px\" >1</button></th>"
          "</tr>"
        "</table>"
        ;
    }
    
    static 
    const String textArea(String handler, String title="", int rows=4, int cols=50 )
    {
      return String() +
      "<form action=\"/"+handler+"\" method=\"post\">"
        + title +
        "<textarea rows=\""+rows+"\" cols=\""+cols+"\" name=\""+title+"_text\">"
        "</textarea>"
        "<br>"
        "<button type=\"submit\">Send</button>"
      "</form>"
      ;
    }

    static 
    const String manualQuery(String data, String bttCaption)
    {
      const String script_getData = String()+
                      "function get_"+data+"(){"
                         "fetch(\"/"+data+"\").then("
                            "function(response){ response.text().then("
                                  "function(text){ document.getElementById(\"manualquery_"+data+"\").innerHTML = text; }"
                      ")})}";
      return "<div>"
               "<button onclick = get_"+data+"()>"+bttCaption+"</button>"
               "<span id=\"manualquery_"+data+"\">init</span>"
             "</div>"
             "<script>"+script_getData+"</script>";
    }

};


#endif