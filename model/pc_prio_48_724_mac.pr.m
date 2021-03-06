MIL_3_Tfile_Hdr_ 145A 140A modeler 9 5DF9DF30 5DF9F8BC 11 DELL victor 0 0 none none 0 0 none BF366EF 1435 0 0 0 0 0 0 1bcc 1                                                                                                                                                                                                                                                                                                                                                                                                    «Ķg      @   D   H      K  o  s  w  {    -  1  ?           	   begsim intrpt             ’’’’      doc file            	nd_module      endsim intrpt             ’’’’      failure intrpts            disabled      intrpt interval         Ō²I­%Ć}’’’’      priority              ’’’’      recovery intrpts            disabled      subqueue         
            count    ’’’   
   ’’’’   
      list   	’’’   
          
   
   super priority             ’’’’                  Packet          *pkptr,*p,*q,*r;   1int channel = 0,size = 0,total = 0,psize = 0,i=0;      S#define ARRIVAL     (op_intrpt_type () == OPC_INTRPT_STRM && op_intrpt_strm() == 0)   S#define SERVICE     (op_intrpt_type () == OPC_INTRPT_STRM && op_intrpt_strm() == 3)                                             J          
   INS_PRIO   
       
      */* a new packet has arrived; acquire it */   &pkptr = op_pk_get (op_intrpt_strm ());           if(pkptr!=OPC_NIL)   {   &	op_pk_fd_get_int32(pkptr,0,&channel);   	op_pk_fd_get_pkt(pkptr,1,&q);   	op_pk_destroy(pkptr);   	   	if(q!=OPC_NIL)   	{   		   		   		   	   ?/* insert the new packet according to priority in subqueue 0 */   Aif (op_subq_pk_insert (channel, q, OPC_QPOS_PRIO) != OPC_QINS_OK)   	{   2	/* if the insertion failed, discard the packet */   	op_pk_destroy (q);   	}       }       }   		       
                         ’’’’             pr_state      	      Z          
   	SEND_HEAD   
       J   2   1/* a request has been made to access the queue */   /* check if its empty */   p = op_pk_get(3);           if(p!=OPC_NIL)   {   "	op_pk_fd_get_int32(p,0,&channel);   	op_pk_fd_get_int32(p,1,&size);   	   	p = op_pk_create(0);   	op_pk_fd_set_int32(p,0,0,32);   	   	total = 32;   	   	i = 0;   	while(!op_subq_empty(channel))   	{   /		q = op_subq_pk_access(channel,OPC_QPOS_HEAD);   "		psize = op_pk_total_size_get(q);   		if(total + psize > size)   		{   		   break;   		}   		   		total+=psize;   		i++;   		    		op_pk_fd_set_pkt(p,i,q,psize);   +		op_subq_pk_remove(channel,OPC_QPOS_HEAD);       	}   	   	if(i>0)   	{   	op_pk_fd_set_int32(p,0,i,32);   	op_pk_send(p,1);   <//	printf("@%lf NEWQ SEND CH = %d\n",op_sim_time(),channel);   	}   	   	   	r = op_pk_create(0);   	for (i = 0; i < 48; i++)   	{   M		op_pk_fd_set_int64(r, i, (long long)op_subq_stat(i,OPC_QSTAT_BITSIZE), 64);   	}   	op_pk_send(r, 2);   	   	   }   J                         ’’’’             pr_state            Ņ          
   BRANCH   
                                   
   ’’’’   
          pr_state                      v  (      }  9   e        ę          
       
       ’’’’          ’’’’          
   0’’’’   
          ’’’’                       pr_transition         	      ¼         ®   k   Į      §   ¹          
       
       ’’’’          ’’’’          
   0’’’’   
          ’’’’                       pr_transition            	   w            “      w          
       
       
   SERVICE   
       ’’’’          
   0’’’’   
          ’’’’                       pr_transition               °           ļ     ,          
       
       
   ARRIVAL   
       ’’’’          
   0’’’’   
          ’’’’                       pr_transition                                    -   General Process Description:    ----------------------------       GThe pc_prio queuing process model accepts packets from any number of sources and holds them until it receives specific requests from another module to forward them. The packets are forwarded in a prioritized (PRIO) manner; highest priority packets are forwarded ahead of earlier arrivals that may still be waiting in the queue.       bThe priority of the packet is determined by the value of the "priority" attribute of each packet.        „The request to forward the packets should come in the form of an "access interrupt" delivered by the module which is the destination module for the concerned packet.               ICI Interfaces:    ---------------       None               Packet Formats:    ---------------        None                Statistic Wires:    ----------------       None                Process Registry:   -----------------        Not Applicable               Restrictions:   -------------        F1. The pc_prio process model must be contained within a queue module.        c2. The source stream index of the output packet stream of the surrounding queue module should be 0.        