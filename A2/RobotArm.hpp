// Includes all source codes for robot Arm rendering and controling 
#pragma once
#include "FruitTetris.hpp"

// Const Parameters Settings that controls the size of the Robot's arm
const GLfloat BASE_HEIGHT      = 2.0;
const GLfloat BASE_WIDTH       = 5.0;
const GLfloat LOWER_ARM_HEIGHT = 10.0;
const GLfloat LOWER_ARM_WIDTH  = 1.0;
const GLfloat UPPER_ARM_HEIGHT = 10.0;
const GLfloat UPPER_ARM_WIDTH  = 1.0;

const int ARMVERTICES = 36;

enum RobotArm_Movement {
	INC_THETA,
	DEC_THETA,
	INC_PHI,
	DEC_PHI
};
// Array of rotation angles (in degrees) for each rotation axis
class RobotArm{
public:
	GLfloat  Phi;
	GLfloat  Theta;

	RobotArm( GLfloat theta = 0.0f, GLfloat phi = 45.0f):Phi(phi), Theta(theta){
	}; 

	void Rotate(RobotArm_Movement direction, GLfloat increment = 1.0f)
	{
		if (direction == INC_PHI)
		{
			this->Phi += increment;
		}
		else if (direction == DEC_PHI)
		{
			this->Phi -= increment;
		}
		else if (direction == INC_THETA)
		{
			this->Theta += increment;
		}
		else if (direction == DEC_THETA)
		{
			this->Theta -= increment;
		}

		if ( this->Phi > 360.0 ) 	{ this->Phi 	-= 360.0; }
		if ( this->Phi < 0.0 )   	{ this->Phi 	+= 360.0; }
		if ( this->Theta > 360.0 ) 	{ this->Theta 	-= 360.0; }
		if ( this->Theta < 0.0 )   	{ this->Theta 	+= 360.0; }

	}

	void drawBase(glm::mat4 model, const GLuint& _loc_model)
	{
	    glm::mat4 instance =	glm::translate( glm::mat4(1.f) , glm::vec3( - 0.5 * BASE_WIDTH, 0.5 * BASE_HEIGHT, 0.0 ) ) * 
	    						glm::scale( glm::mat4(1.f),  glm::vec3( BASE_WIDTH, BASE_HEIGHT, BASE_WIDTH ) ) ;

	    glUniformMatrix4fv( _loc_model, 1, GL_FALSE, glm::value_ptr( model * instance ) );

	    glDrawArrays( GL_TRIANGLES, 0, ARMVERTICES );
	}

	void drawUpperArm(glm::mat4 model, const GLuint& _loc_model)
	{
	    glm::mat4 instance = 	glm::translate( glm::mat4(1.f), glm::vec3( 0.0, BASE_HEIGHT, 0.0) ) *
	    						glm::rotate( glm::mat4(1.f), glm::radians(this->Phi) , glm::vec3(0.0f, 0.0f, 1.0f)) * 
	    						glm::translate( glm::mat4(1.f), glm::vec3( - 0.5 * BASE_WIDTH, 0.5 * UPPER_ARM_HEIGHT, 0.0 ) ) * 
	    						glm::scale( glm::mat4(1.f), glm::vec3( UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH ) );
	    
	    glUniformMatrix4fv( _loc_model, 1, GL_FALSE, glm::value_ptr( model * instance) );
	    glDrawArrays( GL_TRIANGLES, 0, ARMVERTICES );
	}

	void drawLowerArm(glm::mat4 model, const GLuint& _loc_model)
	{
	    glm::mat4 instance = 	glm::translate( glm::mat4(1.f), glm::vec3( 0.0, LOWER_ARM_HEIGHT, 0.0) ) *
	    						glm::rotate( glm::mat4(1.f), glm::radians(this->Theta) , glm::vec3(0.0f, 0.0f, 1.0f)) * 
	    						glm::translate( glm::mat4(1.f), glm::vec3( - 0.5 * BASE_WIDTH, 0.5 * LOWER_ARM_HEIGHT, 0.0 ) ) * 
	    						glm::scale( glm::mat4(1.f), glm::vec3( LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH ) );
	    
	    glUniformMatrix4fv( _loc_model, 1, GL_FALSE, glm::value_ptr( model * instance ) );
	    glDrawArrays( GL_TRIANGLES, 0, ARMVERTICES );
	}

};