// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2014-2018 ANSSI. All Rights Reserved.
#include "emblem-configuration.h"

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include <sys/xattr.h>
#include <string>
#include <vector>
#include <cstdlib>

#include <sys/types.h>
#include <errno.h>
#include <string.h>

namespace emblemConfiguration {

  static const std::string configuration_file_path = chemin;

  Configuration::Configuration() {
  	std::string property_name = "";
    	std::string attribute_name = "";
      	std::string attribute_value = "";
        std::string emblem_name = "";
	}

  Configuration::Configuration(const Configuration& val) {
    property_name = val.property_name;
    attribute_name = val.attribute_name;
    attribute_value = val.attribute_value;
    emblem_name = val.emblem_name;  
  }

  Configuration::~Configuration() {
  }

  TokenPositions::TokenPositions() {
    equal_position = -1 ;
    openingbracket_position = -1;
    closingbracket_position = -1;
    comas_positions[0] = -1;
    comas_positions[1] = -1;
    nb_tokens = 0;
  }

  TokenPositions::~TokenPositions() {
  };


TokenPositions getTokenPositions(std::string line) {
  TokenPositions result;
  int length = line.size();
  
  for (int i = 0; i < length; i++) {
     switch (line.at(i)) {
	case '{' : result.nb_tokens++; result.openingbracket_position = i; break;
	case '}' : result.nb_tokens++; result.closingbracket_position = i; break;
	case '=' : result.nb_tokens++; result.equal_position = i; break;
	case ',' : result.nb_tokens++;
		   if (result.comas_positions[0] == -1) 
		    { result.comas_positions[0] = i; } 
		   else
		    { result.comas_positions[1] = i; };
		   break;
     }
  }
  
  return result;
};


bool isTokenPositionsValid(const TokenPositions& positions) {
  if ((positions.closingbracket_position == -1) 
	|| (positions.openingbracket_position== -1) 
	|| (positions.equal_position == -1) 
	|| (positions.comas_positions[0] == -1)
	|| (positions.comas_positions[1] == -1)
	|| (positions.nb_tokens != 5)) return false;
  
  if (positions.openingbracket_position > positions.closingbracket_position) return false;
  if (positions.equal_position > positions.openingbracket_position) return false;
  if (positions.openingbracket_position > positions.comas_positions[0]) return false;
  if (positions.comas_positions[0] > positions.comas_positions[1]) return false;
  if (positions.comas_positions[1] > positions.closingbracket_position) return false;
  
  return true;
};


bool isConfigurationValid(const Configuration& configuration) {
  if ((configuration.attribute_name == "") 
    || (configuration.attribute_value == "")
    || (configuration.property_name == "")
    || (configuration.emblem_name == ""))
    return false;
  
  return true;
}


std::string removeSpaces(const std::string& line) {
  std::string result;
  
  for (unsigned int i = 0; i < line.size(); i++) {
    char a = line.at(i);
    if (a == ' ')
      continue;
    else
      result+=a;
  }
  
  return result;
};

const Configuration getConfiguration(const std::string& line) {
  Configuration result;
  
  TokenPositions tokenpositions = getTokenPositions(line);
  if (!isTokenPositionsValid(tokenpositions)) {
      std::cout << "can not get the token positions for line : "<< line << std::endl;
      return result;
  }
  
  result.property_name = removeSpaces(line.substr(0, tokenpositions.equal_position));
  result.attribute_name = removeSpaces(line.substr(1+tokenpositions.openingbracket_position, -1 + tokenpositions.comas_positions[0] - tokenpositions.openingbracket_position));
  result.attribute_value = removeSpaces(line.substr(1+tokenpositions.comas_positions[0], -1 + tokenpositions.comas_positions[1] - tokenpositions.comas_positions[0]));  
  result.emblem_name = removeSpaces(line.substr(1+tokenpositions.comas_positions[1], -1 + tokenpositions.closingbracket_position - tokenpositions.comas_positions[1]));
  
  return result;
}


// ---------------------------------------------
// a line is a comment if it starts with "#"
bool isLineAComment(const std::string& line) {
    std::string without_space = removeSpaces(line);
    
    if (without_space.at(0) == '#')
      return true;
        
    return false;  
}

bool isLineEmpty(const std::string& line) {
    std::string without_space = removeSpaces(line);
    if (without_space.size() == 0)
      return true;
    
    return false;
}


// --------------
std::vector<Configuration> parseConfigurationFile() {
  std::vector<Configuration> result;
  std::ifstream configuration_file;
  std::string line_read;
  
  configuration_file.open(configuration_file_path.c_str());
  if (!configuration_file.good())
  	return result;
  
  while (getline(configuration_file, line_read)) {
    // std::cout<<line_read<<std::endl;
    if (isLineAComment(line_read)) continue;
    if (isLineEmpty(line_read)) continue;
    Configuration conf = getConfiguration(line_read);
    if (isConfigurationValid(conf)) result.push_back(conf);
  }
  return result;
}

void printConfiguration(const Configuration& conf) {
    std::cout<<conf.property_name<<"/"<<std::endl;
    std::cout<<conf.attribute_name<<"/"<<std::endl;
    std::cout<<conf.attribute_value<<"/"<<std::endl;
    std::cout<<conf.emblem_name<<"/"<<std::endl;  
}

// ------------------------------------------------------------
// EmblemConfiguration Class ----------------------------------

EmblemConfiguration* EmblemConfiguration::m_instance = NULL;

EmblemConfiguration* EmblemConfiguration::getInstance() {
	if (m_instance == NULL)
		m_instance = new EmblemConfiguration();
	return m_instance;
};


EmblemConfiguration::EmblemConfiguration() {
	configurationsList = parseConfigurationFile();
};

EmblemConfiguration::~EmblemConfiguration() {
};


std::string EmblemConfiguration::getEmblemNameFromAttribute(std::string attribute_name, std::string attribute_value) {

	for (std::vector<Configuration>::const_iterator it = configurationsList.begin(); it != configurationsList.end(); it++) {
		const Configuration& conf = *it;
		if ((conf.attribute_name == attribute_name) && (conf.attribute_value == attribute_value)) { 
			return conf.emblem_name;
			}
	}

	return "";
};

// ----------------------------------------------------------

void setOrigin(std::string path, std::string origin) {
	int result = -1;

	// xxx
	// std::cout<<"origine affectee :" << origin << std::endl;

	if (origin == "aucune") {
		result = removexattr(path.c_str(), field_name.c_str());
		return;
	} else {
		result = setxattr(path.c_str(), field_name.c_str(), (void*) origin.c_str(), origin.size(), XATTR_CREATE);

		if (result == -1) {
			result = setxattr(path.c_str(), field_name.c_str(), (void*) origin.c_str(), origin.size(), XATTR_REPLACE);
		}
	}

	if (result < 0) {
		std::cout << strerror(errno) << std::endl;
	}
}

}
