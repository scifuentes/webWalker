#ifndef WEBELEMENTS_HPP
#define WEBELEMENTS_HPP

#include <ESP8266WebServer.h>
#include <Servo.h> 

struct WebElements
{
    static String servoSlider(int servo_nr,String handler)
    {
        //value show and submitted on change
        return String()+
        "<form action=\""+handler+"\" method=POST oninput=\"x.value=parseInt(servo_"+servo_nr+".value)\" >"
            "Servo "+servo_nr+": 0<input type=\"range\" name=\"servo_"+servo_nr+"\" min=\"0\" max=\"180\" onchange=\"this.form.submit()\">180 : "
            "<output name=\"x\" for=\"servo_"+servo_nr+"\"></output>"
        "</form>";
    }

    static String refreshQuery(String data,int interval)
    {
      const String script_showData = String()+
                   "setInterval("
                      "function(){fetch(\"/"+data+"\").then("
                            "function(response){ response.text().then("
                                  "function(text){ document.getElementById(\"showdata_"+data+"\").innerHTML = text; }"
                         ")})}"
                      ","+interval+");";
      return "<p id=\"showdata_"+data+"\">init</p><script>"+script_showData+"</script>";
    }

    static String browserCounter()
    {
      const String script_Counter = 
                       "var a = 0;"
                       "setInterval("
                         "function()"
                           "{a= a+.1; document.getElementById(\"count\").innerHTML =\"counter: \"+a}"
                         ", 100);";
      return "<p id=\"count\">init</p><script>"+script_Counter+"</script>";
    }




    static String movePad()
    {
        return String()+
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
        "</table>";
    }
    
};



#endif