#include <opnet.h>
#include <ema.h>
#include <opnet_emadefs.h>
#include <opnet_constants.h>

/* array for all objects in model */
EmaT_Object_Id		obj [80];

int
main (int argc, char* argv [])
	{
	EmaT_Model_Id			model_id;
	int					i,j;

	/* initialize EMA package */
	Ema_Init (EMAC_MODE_ERR_PRINT | EMAC_MODE_REL_60, 0, OPC_NIL);

	/* create an empty model */
	model_id = Ema_Model_Create (MOD_ANT_PATTERN);



	/* create all objects */
	for(i=0;i<80;i++){
		obj [i] = Ema_Object_Create (model_id, OBJ_PAT_PLANE);
	}

	/* set the model level attributes */
	Ema_Model_Attr_Set (model_id,
		"num planes",           COMP_CONTENTS, 80,
		EMAC_EOL);

	for(i=0;i<80;i++){
		Ema_Model_Attr_Set (model_id,
		"plane array",          COMP_ARRAY_CONTENTS (i), obj [i],
		EMAC_EOL);
	}

	/* assign attrs for object 'obj [0]' */
	for (i = 0; i < 80; i++){
		Ema_Object_Attr_Set (model_id, obj [i], "gain vec", COMP_DVEC_SIZE, 160, EMAC_EOL);
		Ema_Object_Attr_Set (model_id, obj [i], 
			"view low gain",        COMP_CONTENTS, (double) -0.5,
			"view high gain",       COMP_CONTENTS, (double) 0.5,
			"gain vec",             COMP_INTENDED, EMAC_DISABLED,
			EMAC_EOL);
		for (j = 0; j < 160; j++){
			double value = 0;
			if(i<6){
				value = 30;
			} else {
				value = OPC_DBL_NEG_INFINITY;
			}
			Ema_Object_Attr_Set (model_id, obj [i], "gain vec", COMP_DVEC_CONTENTS(j),value, EMAC_EOL);
		}
	}

	/* write the model to application-readable form */
	Ema_Model_Write (model_id, "directive");

	return 0;
	}