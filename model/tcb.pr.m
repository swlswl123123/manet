MIL_3_Tfile_Hdr_ 145A 140A modeler 9 5B89251E 5E0C3076 24 DELL victor 0 0 none none 0 0 none 13FC89DC 1463 0 0 0 0 0 0 1bcc 1                                                                                                                                                                                                                                                                                                                                                                                                   ��g�      @   D   H      2  ?  C  G  K  W  [  _  &           	   begsim intrpt             ����      doc file            	nd_module      endsim intrpt             ����      failure intrpts            disabled      intrpt interval         ԲI�%��}����      priority              ����      recovery intrpts            disabled      subqueue                     count    ���   
   ����   
      list   	���   
          
      super priority             ����             Packet *	\pac_to_send;       Packet *	\pac_received;       int	\num_of_pac;       Objid	\node_id;       Objid	\proc_id;       unsigned short	\id;          int cur_intrpt_strm;    int cur_intrpt_type;   int cur_intrpt_stat;   const char * TAG = "tcb";      #include <string.h>       /* OPNET system definitions */   #include <opnet.h>   #include <log.h>       /* Header Block */       #define STRM_FROM_ANT_RX 0   #define STRM_TO_ANT_TX 2   #define STRM_TO_TDMA 0   #define STRM_FROM_TDMA 1   #define STAT_FROM_ANT_RX 0   #define STAT_FROM_ANT_TX 1       =#define BUSY_RX (op_stat_local_read(STAT_FROM_ANT_RX) == 1.0)   =#define BUSY_TX (op_stat_local_read(STAT_FROM_ANT_TX) == 1.0)   b#define INTRPT_TDMA_TX (cur_intrpt_type == OPC_INTRPT_STRM) && (cur_intrpt_strm == STRM_FROM_TDMA)   b#define INTRPT_TDMA_RX (cur_intrpt_type == OPC_INTRPT_ACCESS) && (cur_intrpt_strm == STRM_TO_TDMA)   c#define INTRPT_ANT_RX (cur_intrpt_type == OPC_INTRPT_STRM) && (cur_intrpt_strm == STRM_FROM_ANT_RX)                                                �          
   INIT   
       
      proc_id = op_id_self();   "node_id = op_topo_parent(proc_id);   -op_ima_obj_attr_get(node_id, "user id", &id);   num_of_pac = 0;   pac_received = OPC_NIL;   pac_to_send = OPC_NIL;   
                     
   ����   
          pr_state        �   �          
   IDLE   
                     
      #cur_intrpt_type = op_intrpt_type();   +				if (cur_intrpt_type == OPC_INTRPT_STRM)   				{   (					cur_intrpt_strm = op_intrpt_strm();   B					//LOGD("node:%d:%s:intrpt_strm:%d\n",id,TAG,cur_intrpt_strm);   				}   0				else if (cur_intrpt_strm == OPC_INTRPT_STAT)   				{   (					cur_intrpt_stat = op_intrpt_stat();   B					//LOGD("node:%d:%s:intrpt_stat:%d\n",id,TAG,cur_intrpt_stat);   				}   2				else if (cur_intrpt_type == OPC_INTRPT_ACCESS)   				{   (					cur_intrpt_strm = op_intrpt_strm();   D					//LOGD("node:%d:%s:intrpt_access:%d\n",id,TAG,cur_intrpt_strm);   				}   
           ����             pr_state      	  v   �          
   RECV   
       J          +pac_received = op_pk_get(STRM_FROM_ANT_RX);   if (pac_received != OPC_NIL)   {   	//num_of_pac++;   -	op_pk_send_quiet(pac_received,STRM_TO_TDMA);   1	printf("@%.6lf[%d]tcb:RECV\n",op_sim_time(),id);   	pac_received = OPC_NIL;   	// if(num_of_pac>1)   <	// LOGD("[%d:]%s:recv:num of pac is:%d",id,TAG,num_of_pac);   }   J                     
   ����   
          pr_state      
  �  J          
   SEND   
       
      ,//LOGD("node:%d:%s:get_from_tdma\n",id,TAG);   (pac_to_send = op_pk_get(STRM_FROM_TDMA);   
       
   	   $	//LOGD("node:%d:%s:send\n",id,TAG);   	if (pac_to_send != OPC_NIL)   	{   8		// LOGD("node:%d:%s:print the pkt sent out\n",id,TAG);   		//op_pk_print(pac_to_send);   *		op_send_pk(pac_to_send, STRM_TO_ANT_TX);   		pac_to_send = OPC_NIL;   &		// LOGD("node:%d:%s:send\n",id,TAG);   	}   
       
   ����   
          pr_state                       f   �        �  �   �          
   tr_0   
       ����          ����          
    ����   
          ����                       pr_transition            	     �     �   �  f   �          
   tr_20   
       
   INTRPT_ANT_RX   
       ����          
    ����   
          ����                       pr_transition         	     "   �     p   �  �   �          
   tr_21   
       ����          ����          
    ����   
          ����                       pr_transition            
         �   �  �  9          
   tr_24   
       
   INTRPT_TDMA_TX   
       ����          
    ����   
          ����                       pr_transition         
     �       �  ?  �   �          
   tr_25   
       ����          ����          
    ����   
          ����                       pr_transition              j       �   �  �  
  �       �  �   �          
   tr_26   
       
   default   
       ����          
    ����   
          ����                       pr_transition                                             