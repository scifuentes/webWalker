#ifndef COMMANDINTERPRETER_HPP
#define COMMANDINTERPRETER_HPP

#include <vector>
#include <functional>
#include "webHandlers.hpp"

//#define TraceCommands
#ifdef TraceCommands
#define TRACE(foo) Serial.println(foo)
#else
#define TRACE(foo) ;
#endif

typedef std::function<void(const String&, const std::vector<int>&)> HandlerSignature;
struct CommandHandler
{
    String command;
    HandlerSignature method;
};

struct CommandHandlers
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
};

void commandForwarder(const String& command, 
                      const CommandHandlers& cmdHandlers)
{
  TRACE(String(__func__)+": "+command);
 
  if(command.length()==0)
    return;

  std::vector<int> spaces;
  int p=-1;
  do
  {
    p=command.indexOf(" ",p+1);
    if(p>0)
      spaces.push_back(p);
  } while(p>0);
  spaces.push_back(command.length());
  
  String command0 = command.substring(0, spaces[0]);
  TRACE("command0: "+command0);

  for(const CommandHandler& handler: cmdHandlers.handlers)
    if(command0 == handler.command)
    {
        handler.method(command,spaces);
        return;
    }

  TRACE("...Unknown command: '"+command+"'");
}

void commandsSplitter(const String& commands, 
                      const CommandHandlers& cmdHandlers)
{
  //split in lines and send them for execution
  String line;
  line.reserve(commands.length());
  for(int i = 0; i<commands.length(); i++)
    if(commands[i]=='\n')
    {
      commandForwarder(line, cmdHandlers);
      line="";
    } else 
    {
      line+=commands[i];
    }

  if(line.length()>0)
    commandForwarder(line, cmdHandlers);
}

void handleCommandsRequest(ESP8266WebServer& server,
                           const CommandHandlers& cmdHandlers)
{
  server.send(204);
  TRACE(__func__);
  printServerRequest(server);

  commandsSplitter(server.arg(0), cmdHandlers);
}

String listCommands(const CommandHandlers& cmdHandlers, const String spacer="\n")
{
  String commands = "";
  for(const CommandHandler& handler: cmdHandlers.handlers)
    commands += handler.command + spacer;
  return commands;
}






#endif
