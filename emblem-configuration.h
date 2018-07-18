// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2014-2018 ANSSI. All Rights Reserved.
#ifndef EMBLEMCONFIGURATION
#define EMBLEMCONFIGURATION

#include <string>
#include <vector>

namespace emblemConfiguration {

// static const std::string configuration_file_path = "/etc/extended_attributes_configuration.conf";
// static const std::string configuration_file_path = chemin;
static const std::string field_name = "user.clip.origin.level";

static const std::string nom_niveau_haut="DR";
static const std::string nom_niveau_bas="NP";

struct Configuration {
  std::string property_name;
  std::string attribute_name;
  std::string attribute_value;
  std::string emblem_name;
  
  Configuration();
  Configuration(const Configuration& val);
  ~Configuration();

};
 
struct TokenPositions {
  int equal_position;
  int openingbracket_position;
  int closingbracket_position;  
  int comas_positions[2];
  int nb_tokens;

  TokenPositions();
  ~TokenPositions();

};


class EmblemConfiguration {
public :
	static EmblemConfiguration* getInstance();
	std::string getEmblemNameFromAttribute(std::string attribute_name, std::string attribute_value);

private :
	static EmblemConfiguration* m_instance;
	EmblemConfiguration (const EmblemConfiguration&) {};
	EmblemConfiguration& operator=(const EmblemConfiguration&) {return *this;};
	std::vector<Configuration> configurationsList;

	EmblemConfiguration();
	~EmblemConfiguration();
};


 std::vector<Configuration> parseConfigurationFile();
 void setOrigin(std::string path, std::string origin);
}




#endif 
