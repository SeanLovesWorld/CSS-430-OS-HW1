import java.util.Set;
import java.util.TreeSet;

/** Shell.java
 * @author Jeremy Bobotek
 * Shell is a program designed as a handler for programs sent to ThreadOS. It is designed to
 * take in multiple commands in one line and send them to the system to exec(ute).
 * It offers the ability to send multiple processes to the system to run as their own threads
 * simultaneously using the '&' symbol, or to run programs sequentially with the ';' symbol.
 * User should be able to boot up ThreadOS, type l Shell, and then submit a program, such as
 * PingPong, and run it through Shell, and even run multiple copies of it at once or in sequence.
 *
 * The program works by taking input in through a StringBuffer that splits commands based around
 * the delimiters ';' and '&', and passing the results on to SysLib's StringToArgs to execute as
 * an interrupt.
 *
 */

public class Shell extends Thread {


    public Shell()  {

    }

    public Shell(String args[]) {

    }

    public void run(){

        int lineCount = 1;// Shell starts with 1
        while(true){
            SysLib.cout("shell[" + lineCount + "]% ");

            StringBuffer buffer = new StringBuffer();//needed for cin
            SysLib.cin(buffer);

            //send buffer to a String
            String line = new String(buffer);

            if (line.compareTo("exit") == 0){//check for lone exit command
                break;
            }

            if (!line.isEmpty()){//Easy check for an empty line
                lineCount++;
                for (String commands : line.split(";")){//passes off each command(s) delimited by ';' to a parser/runner
                    multiRunner(commands);
                }
            }
        }
        //Exits on loop broken
        SysLib.exit();
    }

    //MultiRunner
    //Allows for concurrent or single programs, by checking for '&' and splitting commands separated by a &
    //Note to self: Concurrent programs have to be kept track of until they complete and can be/are joined
    //              Need to use a data structure to keep track of active tids (Thread IDs)
    //
    public void multiRunner(String commands){
        Set<Integer> threadIDs = new TreeSet<Integer>(); //Set of live tids
        int dummyThreadID;

        //split line by & delimiter, add each delimited command to execution queue
        for(String command: commands.split("&")){

            String[] args = SysLib.stringToArgs(command);//splits String into array for exec to process properly

            if (args.length == 0) {//Checks if the current argument is empty, moves on to next command
                continue;
            }
            else{
                //Start up command/program and adds threadID to set if successfully executed
                if ((dummyThreadID = SysLib.exec(args)) != -1){
                    threadIDs.add(dummyThreadID);
                }
            }
        }

        while(!threadIDs.isEmpty()){//

            dummyThreadID = SysLib.join();//Parent thread waits for each command to end/return
            if (threadIDs.contains(dummyThreadID)){//once command returns, remove from set
                threadIDs.remove(dummyThreadID);
            }
        }
    }
}