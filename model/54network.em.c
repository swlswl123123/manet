#include <opnet.h>
#include <ema.h>
#include <opnet_emadefs.h>
#include <opnet_constants.h>

/* array for all textlist attributes in model */
Prg_List *prg_lptr[8];

/* array for all objects in model */
EmaT_Object_Id obj[59];

int main(int argc, char *argv[])
{
    EmaT_Model_Id model_id;
    int i, id;
    double lat, lon, alt;
    char id_name[256];
    char out_name[256];
    FILE *file;

    /* initialize EMA package */
    Ema_Init(EMAC_MODE_ERR_PRINT | EMAC_MODE_REL_60, 0, NULL);

    /* create an empty model */
    model_id = Ema_Model_Create(MOD_NETWORK);

    /* create all objects */
    obj[0] = Ema_Object_Create(model_id, OBJ_NT_SUBNET_FIX);
    obj[1] = Ema_Object_Create(model_id, OBJ_NT_SUBNET_VIEW);
    obj[2] = Ema_Object_Create(model_id, OBJ_ATTR_PROPS);
    obj[3] = Ema_Object_Create(model_id, OBJ_ATTR_PROPS);
    obj[4] = Ema_Object_Create(model_id, OBJ_ATTR_PROPS);
    obj[5] = Ema_Object_Create(model_id, OBJ_ATTR_PROPS);
    obj[6] = Ema_Object_Create(model_id, OBJ_NT_ISO_ELEV_MAP_COLOR_SETTING);
    obj[7] = Ema_Object_Create(model_id, OBJ_NT_ISO_ELEV_MAP_COLOR_SETTING);
    obj[8] = Ema_Object_Create(model_id, OBJ_NT_ISO_ELEV_MAP_COLOR_SETTING);
    obj[9] = Ema_Object_Create(model_id, OBJ_NT_ISO_ELEV_MAP_COLOR_SETTING);
    obj[10] = Ema_Object_Create(model_id, OBJ_NT_ISO_ELEV_MAP_COLOR_SETTING);

    sprintf(out_name, "%s.csv", argv[1]);
    file = fopen(out_name, "r");
    i = 0;
    while (fscanf(file,"%d,%lf,%lf,%lf", &id, &lat, &lon, &alt) != EOF)
    {   printf("%d,%lf,%lf,%lf\n", id, lat, lon, alt);
        obj[i + 11] = Ema_Object_Create(model_id, OBJ_NT_NODE_MOBILE);
        i++;
    }
    fclose(file);

    /* assign attrs for object 'obj [3]' */
    /* create and init prg list 'prg_lptr [4]' */
    prg_lptr[4] = (Prg_List *)prg_list_create();
    prg_list_strings_append(prg_lptr[4],
                            "This attribute specifies the altitude component of the location of the reference point towards which the antenna pattern is to be directed.",
                            PRGC_NIL);

    Ema_Object_Attr_Set(model_id, obj[2],
                        "units", COMP_CONTENTS, "m",
                        "default value", COMP_CONTENTS_TYPE, EMAC_DOUBLE,
                        "default value", COMP_CONTENTS, (double)0,
                        "comments", COMP_CONTENTS, prg_lptr[4],
                        "symbol map list", COMP_INTENDED, EMAC_DISABLED,
                        "flags", COMP_CONTENTS, 16,
                        "data type", COMP_CONTENTS, 1,
                        "count properties", COMP_INTENDED, EMAC_DISABLED,
                        "list attribute definitions", COMP_INTENDED, EMAC_DISABLED,
                        EMAC_EOL);

    /* assign attrs for object 'obj [4]' */
    /* create and init prg list 'prg_lptr [5]' */
    prg_lptr[5] = (Prg_List *)prg_list_create();
    prg_list_strings_append(prg_lptr[5],
                            "This attribute specifies the latitude component of the location of the reference point towards which the antenna pattern is to be directed.",
                            PRGC_NIL);

    Ema_Object_Attr_Set(model_id, obj[3],
                        "units", COMP_CONTENTS, "deg.",
                        "default value", COMP_CONTENTS_TYPE, EMAC_DOUBLE,
                        "default value", COMP_CONTENTS, (double)0,
                        "comments", COMP_CONTENTS, prg_lptr[5],
                        "high limit", COMP_CONTENTS, (double)90,
                        "low limit", COMP_CONTENTS, (double)-90,
                        "symbol map list", COMP_INTENDED, EMAC_DISABLED,
                        "flags", COMP_CONTENTS, 21,
                        "data type", COMP_CONTENTS, 1,
                        "count properties", COMP_INTENDED, EMAC_DISABLED,
                        "list attribute definitions", COMP_INTENDED, EMAC_DISABLED,
                        EMAC_EOL);

    /* assign attrs for object 'obj [5]' */
    /* create and init prg list 'prg_lptr [6]' */
    prg_lptr[6] = (Prg_List *)prg_list_create();
    prg_list_strings_append(prg_lptr[6],
                            "This attribute specifies the longitude component of the location of the reference point towards which the antenna pattern is to be directed.",
                            PRGC_NIL);

    Ema_Object_Attr_Set(model_id, obj[4],
                        "units", COMP_CONTENTS, "deg.",
                        "default value", COMP_CONTENTS_TYPE, EMAC_DOUBLE,
                        "default value", COMP_CONTENTS, (double)0,
                        "comments", COMP_CONTENTS, prg_lptr[6],
                        "high limit", COMP_CONTENTS, (double)180,
                        "low limit", COMP_CONTENTS, (double)-180,
                        "symbol map list", COMP_INTENDED, EMAC_DISABLED,
                        "flags", COMP_CONTENTS, 21,
                        "data type", COMP_CONTENTS, 1,
                        "count properties", COMP_INTENDED, EMAC_DISABLED,
                        "list attribute definitions", COMP_INTENDED, EMAC_DISABLED,
                        EMAC_EOL);

    /* assign attrs for object 'obj [3]' */
	/* create and init prg list 'prg_lptr [4]' */
	prg_lptr [7] = (Prg_List *)prg_list_create ();
	prg_list_strings_append (prg_lptr [7], 
		"This attribute specifies the Phi component of the reference direction used for pointing the antenna.",
		PRGC_NIL);

	Ema_Object_Attr_Set (model_id, obj [5], 
		"units",                COMP_CONTENTS, "deg.",
		"default value",        COMP_CONTENTS_TYPE, EMAC_DOUBLE,
		"default value",        COMP_CONTENTS, (double) 0,
		"comments",             COMP_CONTENTS, prg_lptr [7],
		"high limit",           COMP_CONTENTS, (double) 180,
		"low limit",            COMP_CONTENTS, (double) 0,
		"symbol map list",      COMP_INTENDED, EMAC_DISABLED,
		"flags",                COMP_CONTENTS, 21,
		"data type",            COMP_CONTENTS, 1,
		"count properties",     COMP_INTENDED, EMAC_DISABLED,
		"list attribute definitions",COMP_INTENDED, EMAC_DISABLED,
		EMAC_EOL);

    /* set the model level attributes */
    /* create and init prg list 'prg_lptr [0]' */
    prg_lptr[0] = (Prg_List *)prg_list_create();
    /* create and init prg list 'prg_lptr [1]' */
    prg_lptr[1] = (Prg_List *)prg_list_create();
    prg_list_strings_append(prg_lptr[1],
                            "custom_model_list",
                            "internet_toolbox",
                            PRGC_NIL);

    /* create and init prg list 'prg_lptr [2]' */
    prg_lptr[2] = (Prg_List *)prg_list_create();
    Ema_Model_Attr_Set(model_id,
                       "ext fileset", COMP_CONTENTS, prg_lptr[0],
                       "keywords list", COMP_CONTENTS, prg_lptr[1],
                       "view subnet", COMP_CONTENTS, obj[0],
                       "iso elev map color levels", COMP_ARRAY_CONTENTS(0), obj[6],
                       "iso elev map color levels", COMP_ARRAY_CONTENTS(1), obj[7],
                       "iso elev map color levels", COMP_ARRAY_CONTENTS(2), obj[8],
                       "iso elev map color levels", COMP_ARRAY_CONTENTS(3), obj[9],
                       "iso elev map color levels", COMP_ARRAY_CONTENTS(4), obj[10],
                       "iso elev map label color", COMP_CONTENTS, 0,
                       "view index list", COMP_CONTENTS, prg_lptr[2],
                       EMAC_EOL);

    /* assign attrs for object 'obj [0]' */
    /* create and init prg list 'prg_lptr [3]' */
    prg_lptr[3] = (Prg_List *)prg_list_create();
    Ema_Object_Attr_Set(model_id, obj[0],
                        "name", COMP_CONTENTS, "top",
                        "name", COMP_USER_INTENDED, EMAC_ENABLED,
                        "x position", COMP_CONTENTS, (double)0,
                        "x position", COMP_USER_INTENDED, EMAC_ENABLED,
                        "y position", COMP_CONTENTS, (double)0,
                        "y position", COMP_USER_INTENDED, EMAC_ENABLED,
                        "x span", COMP_CONTENTS, (double)360,
                        "x span", COMP_USER_INTENDED, EMAC_ENABLED,
                        "y span", COMP_CONTENTS, (double)180,
                        "y span", COMP_USER_INTENDED, EMAC_ENABLED,
                        "icon name", COMP_CONTENTS, "subnet",
                        "icon name", COMP_INTENDED, EMAC_DISABLED,
                        "map", COMP_CONTENTS, "world",
                        "map", COMP_USER_INTENDED, EMAC_ENABLED,
                        "subnet", COMP_CONTENTS, EMAC_NULL_OBJ_ID,
                        "view stack", COMP_ARRAY_CONTENTS(0), obj[1],
                        EMAC_EOL);

    Ema_Object_Attr_Set(model_id, obj[0],
                        "grid unit", COMP_CONTENTS, 5,
                        "ui status", COMP_CONTENTS, 0,
                        "iso-elev-map list", COMP_CONTENTS, prg_lptr[3],
                        "iso-elev-map line threshold", COMP_CONTENTS, (double)50,
                        "view", COMP_CONTENTS, "Default View",
                        "view mode", COMP_CONTENTS, 0,
                        "view positions", COMP_INTENDED, EMAC_DISABLED,
                        EMAC_EOL);

    file = fopen(out_name, "r");
    i = 0;
    while (fscanf(file,"%d,%lf,%lf,%lf", &id, &lat, &lon, &alt) != EOF)
    {

        sprintf(id_name, "%d", id);
        /* assign attrs for object 'obj [2]' */
        Ema_Object_Attr_Set(model_id, obj[11+i],
                            "name", COMP_CONTENTS, id_name,
                            "name", COMP_USER_INTENDED, EMAC_ENABLED,
                            "model", COMP_CONTENTS, "724node",
                            "model", COMP_USER_INTENDED, EMAC_ENABLED,
                            "user id", COMP_CONTENTS, id,
                            "user id", COMP_USER_INTENDED, EMAC_ENABLED,
                            "x position", COMP_CONTENTS, (double)lon,
                            "x position", COMP_USER_INTENDED, EMAC_ENABLED,
                            "y position", COMP_CONTENTS, (double)lat,
                            "y position", COMP_USER_INTENDED, EMAC_ENABLED,
                            "altitude", COMP_CONTENTS, (double)alt,
                            "altitude", COMP_USER_INTENDED, EMAC_ENABLED,
                            "trajectory", COMP_CONTENTS, "NONE",
                            "trajectory", COMP_INTENDED, EMAC_DISABLED,
                            "trajectory", COMP_USER_INTENDED, EMAC_ENABLED,
                            "ground speed", COMP_CONTENTS, "",
                            EMAC_EOL);

        Ema_Object_Attr_Set(model_id, obj[11+i],
                            "ground speed", COMP_INTENDED, EMAC_DISABLED,
                            "ground speed", COMP_USER_INTENDED, EMAC_ENABLED,
                            "ascent rate", COMP_CONTENTS, "",
                            "ascent rate", COMP_INTENDED, EMAC_DISABLED,
                            "ascent rate", COMP_USER_INTENDED, EMAC_ENABLED,
                            "color", COMP_CONTENTS, 1090519039,
                            "color", COMP_INTENDED, EMAC_DISABLED,
                            "color", COMP_USER_INTENDED, EMAC_ENABLED,
                            "doc file", COMP_CONTENTS, "",
                            "doc file", COMP_INTENDED, EMAC_DISABLED,
                            "doc file", COMP_USER_INTENDED, EMAC_ENABLED,
                            "subnet", COMP_CONTENTS, obj[0],
                            "alias", COMP_INTENDED, EMAC_DISABLED,
                            "tooltip", COMP_CONTENTS, "",
                            "tooltip", COMP_INTENDED, EMAC_DISABLED,
                            "tooltip", COMP_USER_INTENDED, EMAC_ENABLED,
                            EMAC_EOL);

        Ema_Object_Attr_Set(model_id, obj[11+i],
                            "ui status", COMP_CONTENTS, 512,
                            "view positions", COMP_INTENDED, EMAC_DISABLED,
                            EMAC_EOL);

        i++;
    }
    fclose(file);
    /* assign attrs for object 'obj [1]' */
    Ema_Object_Attr_Set(model_id, obj[1],
                        "min x", COMP_CONTENTS, (double)-182.4,
                        "min y", COMP_CONTENTS, (double)90.4,
                        "sbar x", COMP_CONTENTS, (double)0,
                        "sbar y", COMP_CONTENTS, (double)0,
                        "grid step", COMP_CONTENTS, (double)15,
                        "resolution", COMP_CONTENTS, (double)2.5,
                        "grid style", COMP_CONTENTS, 2,
                        "grid color", COMP_CONTENTS, 21,
                        EMAC_EOL);

    /* assign attrs for object 'obj [6]' */
    Ema_Object_Attr_Set(model_id, obj[5],
                        "elevation", COMP_CONTENTS, (double)1e+100,
                        "color", COMP_CONTENTS, 1090519039,
                        EMAC_EOL);

    /* assign attrs for object 'obj [7]' */
    Ema_Object_Attr_Set(model_id, obj[6],
                        "elevation", COMP_CONTENTS, (double)5000,
                        "color", COMP_CONTENTS, 1073741824,
                        EMAC_EOL);

    /* assign attrs for object 'obj [8]' */
    Ema_Object_Attr_Set(model_id, obj[7],
                        "elevation", COMP_CONTENTS, (double)3000,
                        "color", COMP_CONTENTS, 1086806624,
                        EMAC_EOL);

    /* assign attrs for object 'obj [9]' */
    Ema_Object_Attr_Set(model_id, obj[8],
                        "elevation", COMP_CONTENTS, (double)1000,
                        "color", COMP_CONTENTS, 1073745152,
                        EMAC_EOL);

    /* assign attrs for object 'obj [10]' */
    Ema_Object_Attr_Set(model_id, obj[9],
                        "elevation", COMP_CONTENTS, (double)1,
                        "color", COMP_CONTENTS, 1079591136,
                        EMAC_EOL);

    file = fopen(out_name, "r");
    i = 0;
    while (fscanf(file,"%d,%lf,%lf,%lf", &id, &lat, &lon, &alt) != EOF)
    {

        /* promoted attrs for object 'obj [2]' */
        Ema_Object_Prom_Attr_Set(model_id, obj[i + 11],
                                 "ar.target altitude", obj[2]);
        Ema_Object_Prom_Attr_Set(model_id, obj[i + 11],
                                 "ar.target latitude", obj[3]);
        Ema_Object_Prom_Attr_Set(model_id, obj[i + 11],
                                 "ar.target longitude", obj[4]);
        Ema_Object_Prom_Attr_Set (model_id, obj [i + 11], 
		                        "ar.pointing ref. phi", obj [5]);
        Ema_Object_Attr_Set(model_id, obj[i + 11],
                            "ar.target altitude", COMP_PROMOTE, EMAC_DISABLED,
                            "ar.target altitude", COMP_CONTENTS, (double)0,
                            "ar.target altitude", COMP_USER_INTENDED, EMAC_ENABLED,
                            "ar.target latitude", COMP_PROMOTE, EMAC_DISABLED,
                            "ar.target latitude", COMP_CONTENTS, (double)0,
                            "ar.target latitude", COMP_USER_INTENDED, EMAC_ENABLED,
                            "ar.target longitude", COMP_PROMOTE, EMAC_DISABLED,
                            "ar.target longitude", COMP_CONTENTS, (double)0,
                            "ar.target longitude", COMP_USER_INTENDED, EMAC_ENABLED,
                            "ar.pointing ref. phi", COMP_PROMOTE, EMAC_DISABLED,
                            "ar.pointing ref. phi", COMP_CONTENTS, (double) 21.0906,
                            "ar.pointing ref. phi", COMP_USER_INTENDED, EMAC_ENABLED,
                            EMAC_EOL);
        i++;
    }
    fclose(file);

    /* write the model to application-readable form */
    Ema_Model_Write(model_id, argv[1]);

    return 0;
}
