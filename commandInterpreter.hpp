#ifndef COMMANDINTERPRETER_HPP
#define COMMANDINTERPRETER_HPP

#include <vector>
#include <functional>

typedef std::function<void(const String&, const std::vector<int>&)> HandlerSignature;
struct CommandHandler
{
    String command;
    HandlerSignature method;
};


struct CommandInterpreter
{
    std::vector<CommandHandler> handlers;

    void add(const String& command, HandlerSignature method)
    {
        handlers.push_back(CommandHandler{command,method});
    }
    void add(const String& command, std::function<void()> method)
    {
        handlers.push_back(CommandHandler{command, [method](const String& command, const std::vector<int>&){method();} });
    }

    void go(const String& command)
    {
      Serial.println(String(__func__)+": "+command);
      
      std::vector<int> spaces;
      int p=-1;
      do
      {
        p=command.indexOf(" ",p+1);
        if(p>0)
          spaces.push_back(p);
      } while(p>0); 
      
      for(const CommandHandler& handler: handlers)
      {
        String command0 = command.substring(0, spaces[0]);
        if(command0 == handler.command)
        {
            handler.method(command,spaces);
            return;
        }
      }

      Serial.println("Unknown command");
    }


};

CommandInterpreter interpreter;

void commandInterpreter(const String& command)
{
    interpreter.go(command);
}


#endif
