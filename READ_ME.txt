Creators of this newshell: Timothy Stern, Ramiro Ortiz, Ashley Torres, & Read Ballew

Organization  of  the  Project:

    The organization of this project was simple. Tim and Read worked on parse.h and major1.c, while Ramiro and Ashley worked on pipelining.h and redirection.h
    respectively.
    
    Tim handled the input and sorting the input in to comprehensible and usable strings.
    
    Read worked on built in commands, signal handling, and the bonus objectives.
    
    Ramiro handled the pipelining and the file redirection inside pipelining.
    
    Ashley handled the redirection and the file redirection inside pipelining.
    
This worked well, because Tim had the best working version of minor 5 leading into the project, Read understood signal handling and foreground processes,
Ramiro understood pipelining and the requirements that came with it, and Ashley undertood the ramifications of file redirection.

Design  Overview:  

    Firstly, the program is designed to be worked on by multiple parties with little interference. It is separated to the best of our ability, letting each
    creator have little to no impact on each other's working pieces. This made the general project design smooth, and versatile.
    
    Now, the code structure: major1.c handles the large loop for terminal and batch mode. In terminal mode, it prints the custom option first, 
    having you name your very own prompt! Once you have named the prompt, it enters an unending loop, waiting for the exit call. Inside this loop, it
    prints the custom prompt, and calls the prase function inside parse.h. The function is passed all input from that line where it is further broken down
    using string tokens and character checking.
    
    When parsing for input, the pipelining and redirection functions are called. They are passed only the information they need to operate, and are passed
    the left, center, or right side of the operation based on what kind of operation is called upon. At this point, the only commands left are basic and
    built in commands; these commands are handled separately.
    
    There are custom checks for each of the requested built in commands, and after the input is parsed for those commands, it is forked to create separate
    processes that can be affected by signals (unlike the newshell process). These processes can be killed or altered using signals.

Complete Specification: 

    The custom bonus prompt is forced, that way you name your prompt and hopefully feel too attached to the shell to kill it.
    
    If semi colon comes through with command preceding it, we simply skip executing commands for that command placement.
    
    Shell history is output to history.txt using a simple file pointer that is opened and closed all inside the parse function.
    
    
Known Bugs or Problems:

    