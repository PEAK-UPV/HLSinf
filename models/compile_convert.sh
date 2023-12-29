cc -c fig.c            
cc -c in_out.c        
cc -c initializers.c  
cc -c main.c          
cc -c nodes.c         
cc -c parsers.c       
cc -c utils.c         
cc -c model.c         
cc fig.o in_out.o nodes.o initializers.o main.o model.o parsers.o utils.o -o hlsinf_runner
