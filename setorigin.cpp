// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2014-2018 ANSSI. All Rights Reserved.
#include "emblem-configuration.h"
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

#include <sys/types.h>
#include <attr/xattr.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

using namespace emblemConfiguration;

/*
static std::string script_base_debut_xdialog = std::string(" Xdialog --title \"Message Box\" ")+ " --menu \"Contexte d'origine\" 10 41 ";
static std::string script_base_fin_xdialog = std::string(" 2> /tmp/menu.tmp.$$ \n")
+ " " 
+ "retval=$? \n" 
+ "choice=`cat /tmp/menu.tmp.$$` \n"
+ "rm -f /tmp/menu.tmp.$$ \n"
+ " "
+ " case $retval in \n"
+ " 0) \n"
+ " echo \"$choice\";; \n"
+ " 1) \n"
+ " echo \"annulé\";; \n"
+ " 255) \n"
+ " echo \"box closed.\";; \n"
+ "esac";
*/

static std::string script_base_debut = std::string("kdialog --menu \"Sensibilite\" ");
static std::string script_base_fin = std::string(" ");


std::string askUserForOrigin() {
	FILE *in;
	char buff[512];

	std::vector<Configuration> res = parseConfigurationFile();
	std::vector<Configuration>::const_iterator i;
	std::vector<std::string> choix_possibles;
	std::string argument = "";
	std::string returned_value = "";
	char indexchar[10];

	int index = 0;
	int numero_choix = 0;

	for (i = res.begin(); i != res.end(); i++) {
		index++;
		Configuration conf = *i;
		char indexchar[10];
		if (conf.attribute_name != field_name)
			continue;
		snprintf(indexchar, 10, "%d", index);
		// xdialog : argument += " \"" + std::string(indexchar) + "\" "  + "\"" + conf.attribute_value+"\"";	
		argument += std::string(indexchar) + " " + conf.attribute_value + " ";
		choix_possibles.push_back(conf.attribute_value);

		// test
		std::cout<<conf.attribute_value<<std::endl;
	}
	
	// ajout du choix de retrait de l'origine
	index++;
	snprintf(indexchar, 10, "%d", index);
	argument += std::string(indexchar) + " inconnue ";
	choix_possibles.push_back("inconnue");

	if (choix_possibles.size() == 0)
		return "";

	std::string commande = script_base_debut + argument + " " + script_base_fin;
	
	if (!(in = popen(commande.c_str(), "r"))) {
		return "";
	}

	while(fgets(buff, sizeof(buff), in) != NULL) {
		returned_value += std::string(buff);
	}

	pclose(in);

	// if (returned_value == "annulé")
	if (returned_value == "")
		return "";

	numero_choix = atoi(returned_value.data());

	// xxx
	// std::cout << returned_value.data() << std::endl;

	if (numero_choix < 1)
		return "";

	returned_value = choix_possibles.at(numero_choix - 1);

	// xxx
	// std::cout << "returned value = " << returned_value.data() << std::endl;

	return returned_value;
}

int main(int argc, char** argv) {
	std::vector<std::string> fichiers;
	int fichier_start_index = 1;
	std::string origine = "";


	// test if kdialog utility exists
	if ( access("/usr/local/bin/kdialog", F_OK) == -1 ) {
		// file "/usr/local/bin/kdialog" is not found
		std::cout << "This program requires kdialog to be on /usr/local/bin path" << std::endl;
		return 0;
	}

	if (argc <= 1) {
		std::cout << "type -h or --help for help" << std::endl;
		return 0;
	}

	if ((std::string("-h") == argv[1]) || (std::string("--help") == argv[1])) {
		std::cout << "usage : setorigin [FILES]" << std::endl;
		std::cout << "usage : setorigin -o [Origin] [FILES]" << std::endl;
		return 0;
	}

	if (std::string("-o") == argv[1]) {
		fichier_start_index = 3;
		origine = argv[2];
	} else {
		origine = askUserForOrigin();
	}

	for (int i = fichier_start_index; i < argc; i++) {
		fichiers.push_back(argv[i]);
	}

	if (origine == "") return 0;

	std::cout<< "etiquette de l'origine : " << origine<<std::endl;
	
	for(std::vector<std::string>::const_iterator it = fichiers.begin(); it != fichiers.end(); it++) {
		setOrigin(*it, origine);
		std::cout<< "fichier modifié : " << *it<<std::endl;
	}

  	return 0;
}

