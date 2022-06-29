#pragma once

#include <string>
#include <thread>
#include <mutex>
#include "../include/API_scripting.h"
#include "../../../AMLib/include/Database_implementations/Data_stuctures/DBS_All_Structures_Header.h"
#include "../../../AMLib/interfaces/IAM_lua_functions.h"
#include "../../../AMLib/include/AM_Config.h"
#include "../../../AMLib/interfaces/IAM_Database.h"
#include "../../../AMLib/x_Helpers/IPC_winapi.h"
#include "API_matcalc_lib.h"

/** \addtogroup AM_API_lib
  *  @{
  */

class API_lua_functions: public IAM_lua_functions
{
public:

	API_lua_functions(lua_State* state);
	API_lua_functions(lua_State* state, AM_Config* configuration);
	~API_lua_functions();
	static void set_library(AM_Config* configuration);

private:
	inline static API_matcalc_lib* _api{nullptr};
	inline static std::mutex _mutex;

	/// <summary>
	/// Registers the function's signature in lua 
	/// </summary>
	/// <param name="state"></param>
	virtual void add_functions_to_lua(lua_State* state) override;

#pragma region lua_functions
	/// <summary>
	/// This is just an example of how you should bind a function in lua
	/// so I'll call this baby lua.
	/// </summary>
	/// <param name="state"></param>
	/// <returns>number of parameters it outputs</returns>
	static int bind_hello_world(lua_State* state);

	/// <summary>
	/// closes matcalc core
	/// </summary>
	/// <param name="state"></param>
	/// <returns></returns>
	static int bind_exit(lua_State* state);

	/// <summary>
	/// This calls the run matcalc script, the parameter passed to lua has to be 
	/// the filename for the script
	/// </summary>
	/// <param name="state"></param>
	/// <returns></returns>
	static int bind_run_script(lua_State* state);

	/// <summary>
	/// runs specific commands using matcalc scripting commands
	/// </summary>
	/// <param name="state"></param>
	/// <returns></returns>
	static int bind_run_command(lua_State* state);

	/// <summary>
	/// Initializes matcalc core module, loads databases and all
	/// for initialization. This function also loads into the
	/// database all elements and phases, ref: bind_getElementNames_command
	/// and bind_getPhaseNames_command
	/// </summary>
	/// <param name="state"></param>
	/// <returns></returns>
	static int bind_initializeCore_command(lua_State* state);

	/// <summary>
	/// Returns Database element list
	/// </summary>
	/// <param name="state"></param>
	/// <returns></returns>
	static int bind_DatabaseElementNames_command(lua_State* state);

	/// <summary>
	/// Obtains the elements available on a database
	/// </summary>
	/// <param name="state"></param>
	/// <returns></returns>
	static int bind_getElementNames_command(lua_State* state);

	/// <summary>
	/// get phases available in the database
	/// </summary>
	/// <param name="state"></param>
	/// <returns></returns>
	static int bind_DatabasePhaseNames_command(lua_State* state);

	/// <summary>
	/// Obtains the elements available on a database
	/// </summary>
	/// <param name="state"></param>
	/// <returns></returns>
	static int bind_getPhaseNames_command(lua_State* state);

	/// <summary>
	/// set the number of precipitation classes, 
	/// by default this is set to 25.
	/// </summary>
	/// <param name="state"></param>
	/// <returns></returns>
	static int bind_setValueNPC_command(lua_State* state);

	/// <summary>
	/// set path to thermodynamic database
	/// </summary>
	/// <param name="state"></param>
	/// <returns></returns>
	static int bind_selectThermodynamicDatabase_command(lua_State* state);

	/// <summary>
	/// set path to physical database
	/// </summary>
	/// <param name="state"></param>
	/// <returns></returns>
	static int bind_selectPhysicalDatabase_command(lua_State* state);

	/// <summary>
	/// set path to mobility database
	/// </summary>
	/// <param name="state"></param>
	/// <returns></returns>
	static int bind_selectMobilityDatabase_command(lua_State* state);

	/// <summary>
	/// Select phases for calculations
	/// </summary>
	/// <param name="state"></param>
	/// <returns></returns>
	static int bind_selectPhases_command(lua_State* state);

	/// <summary>
	/// calculate equilibrium
	/// </summary>
	/// <param name="state"></param>
	/// <returns></returns>
	static int bind_calculateEquilibrium_command(lua_State* state);

#pragma region MatCalc

	static int bind_matcalc_initializeCore(lua_State* state)
	{
		std::string out = _api->APIcommand(API_Scripting::script_initialize_core());

		lua_pushstring(state, out.c_str());
		return 1;
	}

	static int bind_matcalc_setWorkingDirectory(lua_State* state)
	{
		check_parameters(state, lua_gettop(state), 0, "No path added");
		std::vector<std::string> parameters = get_parameters(state);

		if (parameters.size() > 0)
			_configuration->set_working_directory(IAM_Database::csv_join_row(parameters, " "));

		std::string out = _api->APIcommand(API_Scripting::script_set_working_directory(_configuration));

		lua_pushstring(state, out.c_str());
		return 1;
	}

	static int bind_matcalc_openThermodynamicDatabase(lua_State* state)
	{
		check_parameters(state, lua_gettop(state), 0, "you are golden");
		std::vector<std::string> parameters = get_parameters(state);

		if(parameters.size() > 0)
			_configuration->set_ThermodynamicDatabase_path(IAM_Database::csv_join_row(parameters, " "));
		
		std::string out = _api->APIcommand(API_Scripting::script_set_thermodynamic_database(_configuration->get_ThermodynamicDatabase_path()));

		lua_pushstring(state, out.c_str());
		return 1;
	}

	static int bind_matcalc_selectElements(lua_State* state)
	{
		check_parameters(state, lua_gettop(state), 0, "you are golden");
		std::vector<std::string> parameters = get_parameters(state);

		if (parameters.size() > 0)
			_openProject->set_selected_elements_ByName(parameters);

		std::string out = _api->APIcommand(API_Scripting::Script_selectElements(_openProject->get_selected_elements_ByName()));

		lua_pushstring(state, out.c_str());
		return 1;
	}

	static int bind_matcalc_selectPhases(lua_State* state)
	{
		check_parameters(state, lua_gettop(state), 1, "Please select at least one phase");
		std::vector<std::string> parameters = get_parameters(state);

		std::string out = _api->APIcommand(API_Scripting::Script_selectPhases(parameters));

		lua_pushstring(state, out.c_str());
		return 1;
	}

	static int bind_matcalc_readThermodynamicDatabase(lua_State* state)
	{
		check_parameters(state, lua_gettop(state), 0, "you are golden");
		std::vector<std::string> parameters = get_parameters(state);

		std::string out = _api->APIcommand(API_Scripting::Script_readThermodynamicDatabase());

		lua_pushstring(state, out.c_str());
		return 1;
	}

	static int bind_matcalc_setReferenceElement(lua_State* state)
	{
		check_parameters(state, lua_gettop(state), 1, "Please add a reference element");
		std::vector<std::string> parameters = get_parameters(state);

		std::string out = _api->APIcommand(API_Scripting::Script_setReferenceElement(parameters[0]));

		lua_pushstring(state, out.c_str());
		return 1;
	}

	static int bind_matcalc_setStepOption(lua_State* state)
	{
		check_parameters(state, lua_gettop(state), 1, "Please add a reference element");
		std::vector<std::string> parameters = get_parameters(state);

		std::string out = _api->APIcommand(API_Scripting::Script_setStepOptions(IAM_Database::csv_join_row(parameters, " ")));

		lua_pushstring(state, out.c_str());
		return 1;
	}

	static int bind_matcalc_stepEquilibrium(lua_State* state)
	{
		check_parameters(state, lua_gettop(state), 0, "you are golen!");
		std::vector<std::string> parameters = get_parameters(state);

		std::string out = _api->APIcommand(API_Scripting::Script_stepEquilibrium());

		lua_pushstring(state, out.c_str());
		return 1;
	}

	/// <summary>
	/// Obtains the phase cumulative fraction for the equilibrium calculations
	/// </summary>
	/// <param name="state"></param>
	/// <returns></returns>
	static int bind_matcalc_buffer_getEquilibriumPhaseFraction(lua_State* state)
	{
		// we require at least one phase.
		check_parameters(state, lua_gettop(state), 1, "please add a phase");
		std::vector<std::string> parameters = get_parameters(state);

		// construct for string format all double entries
		std::string formatVariable = string_manipulators::get_string_format_numeric_generic(parameters.size(), "%g", ",");

		// set matcalc variable name
		std::string varName = "EquilibriumAM";

		// create a variable in matcalc using string format
		std::string sfv = _api->APIcommand(API_Scripting::script_format_variable_string(varName,
																						formatVariable, 
																						API_Scripting::script_get_phase_equilibrium_variable_name(parameters)));
		// get string by printing to console
		std::string out = _api->APIcommand(API_Scripting::print_variable_to_console(varName));
		out = string_manipulators::ltrim_byToken(out, " = ");
		out = string_manipulators::rtrim_byToken(out, " ");

		lua_pushstring(state, out.c_str());
		return 1;
	}

	/// <summary>
	/// Obtains the cumulative fraction for the scheil equilibrium calculations
	/// </summary>
	/// <param name="state"></param>
	/// <returns></returns>
	static int bind_matcalc_buffer_getScheilPhaseFraction(lua_State* state)
	{
		// we require at least one phase.
		check_parameters(state, lua_gettop(state), 1, "please add a phase");
		std::vector<std::string> parameters = get_parameters(state);

		// construct for string format all double entries
		std::string formatVariable = string_manipulators::get_string_format_numeric_generic(parameters.size(), "%g", ",");

		// set matcalc variable name
		std::string varName = "EquilibriumAM";

		// create a variable in matcalc using string format
		std::string sfv = _api->APIcommand(API_Scripting::script_format_variable_string(varName,
																						formatVariable,
																						API_Scripting::script_get_phase_equilibrium_scheil_variable_name(parameters)));
		// get string by printing to console
		std::string out = _api->APIcommand(API_Scripting::print_variable_to_console(varName));

		lua_pushstring(state, out.c_str());
		return 1;
	}

	/// <summary>
	/// matcalc function returns buffer content
	/// </summary>
	/// <param name="state"></param>
	/// <returns></returns>
	static int bind_matcalc_buffer_listContent(lua_State* state)
	{
		check_parameters(state, lua_gettop(state), 0, "you are golden");
		/*std::string out = _api->APIcommand(API_Scripting::script_buffer_listContent());
		int index_start_buffer = string_manipulators::find_index_of_keyword(out, "Index");
		int index_end_buffer = string_manipulators::find_index_of_keyword(out, "MC:");
		out = out.substr(index_start_buffer, index_end_buffer - index_start_buffer - 2);
		*/
		std::string out = matcalc_buffer_listContent();
		lua_pushstring(state, out.c_str());
		return 1;
	}
	static std::string matcalc_buffer_listContent(IPC_winapi* mccComm = nullptr) 
	{
		std::string out;
		if(mccComm == nullptr)
			out = _api->APIcommand(API_Scripting::script_buffer_listContent());
		else
			out = _api->APIcommand(API_Scripting::script_buffer_listContent(), mccComm);

		int index_start_buffer = string_manipulators::find_index_of_keyword(out, "Index");
		int index_end_buffer = string_manipulators::find_index_of_keyword(out, "MC:");
		return out.substr(index_start_buffer, index_end_buffer - index_start_buffer - 2);
	}

	/// <summary>
	/// Clears the buffer content
	/// </summary>
	/// <param name="state"></param>
	/// <returns></returns>
	static int bind_matcalc_buffer_clear(lua_State* state)
	{
		check_parameters(state, lua_gettop(state), 1, "please add the buffer name");
		std::vector<std::string> parameters = get_parameters(state);

		std::string out = _api->APIcommand(API_Scripting::script_buffer_clear(parameters[0]));
		lua_pushstring(state, out.c_str());
		return 1;
	}

#pragma endregion

#pragma region Project

	/// <summary>
	/// Based on the current openProject, runs a stepped equilibrium calculation
	/// input Parameters: INT <IDCase>
	/// </summary>
	/// <param name="state"></param>
	/// <returns></returns>
	static int Bind_get_active_phases(lua_State* state)
	{
		//check for input
		if (check_parameters(state, lua_gettop(state), 1, "usage <ID Case>") != 0) return 1; 
		
		// get parameters
		std::vector<std::string> parameters = get_parameters(state);

		// clear buffer before starting
		std::string outClearBuffer = run_command(state, "matcalc_buffer_clear", std::vector<std::string> {"_default_"});
		std::string phaseNames = run_command(state, "matcalc_database_phaseNames");
		std::vector<std::string> listPhases = string_manipulators::split_text(phaseNames, "\n");
		std::string idCase = parameters[0];
		
		// If pixel_parameters is a null pointer, that means that either the case does not exist or
		// it corresponds to another project ID
		AM_pixel_parameters* pixel_parameters = _openProject->get_pixelCase(std::stoi(idCase));
		if (pixel_parameters == nullptr)
		{
			lua_pushstring(state, "Error: IDCase does not belong to the project or it does not exist!");
			return 1;
		}

		// Initialize case
		//TODO: When no selected elements, gives Error, catch this one
		std::string outCommand_1 = runVectorCommands(API_Scripting::Script_run_stepEquilibrium(_configuration,
			pixel_parameters->get_EquilibriumConfiguration()->StartTemperature,
			pixel_parameters->get_EquilibriumConfiguration()->EndTemperature,
			_openProject->get_selected_elements_ByName(),
			pixel_parameters->get_composition_string(),
			listPhases));

		// Get buffer content
		std::string buffer_raw = run_command(state, "matcalc_buffer_listContent");
		std::vector<std::string> bufferRowEntries = string_manipulators::split_text(buffer_raw, "\n");
		if (bufferRowEntries.size() < 2)
		{
			std::string ErrorOut = "Error: Equilibrium calculation was not possible -> " + outCommand_1;
			lua_pushstring(state, ErrorOut.c_str());
			return 1;
		}

		// pixel parameters
		std::vector<std::string> selectedPhases = pixel_parameters->get_selected_phases_ByName();
		std::vector<int> selectedPhases_id = pixel_parameters->get_selected_phases_ByID();
		std::vector<IAM_DBS*> tempPhaseFraction(listPhases.size());

		for (size_t n1 = 0; n1 < listPhases.size(); n1++)
		{
			tempPhaseFraction[n1] = new DBS_ActivePhases(_dbFramework->get_database(), -1);
		}

		// retreive data from matcalc buffer
		for (int n1 = 1; n1 < bufferRowEntries.size(); n1++)
		{
			// get buffer row items
			std::vector<std::string> bufferCells = string_manipulators::split_text(bufferRowEntries[n1], " ");
			std::string outCommand_loadState = _api->APIcommand(API_Scripting::script_buffer_loadState(n1 - 1));

			// get phase fraction values in a csv format, delimiter is ","
			std::string outCommand_statusPhase = run_command(state, "matcalc_buffer_get_equilibrium_phase_fraction", selectedPhases);
			std::vector<std::string> phaseValues = string_manipulators::split_text(outCommand_statusPhase, ",");

			for (int n2 = 0; n2 < selectedPhases.size(); n2++)
			{
				int indexPhase = (n1 - 1) * selectedPhases.size() + n2;
				((DBS_EquilibriumPhaseFraction*)tempPhaseFraction[indexPhase])->IDCase = pixel_parameters->get_caseID();
				((DBS_EquilibriumPhaseFraction*)tempPhaseFraction[indexPhase])->IDPhase = selectedPhases_id[n2];
			}

			if (bufferCells.size() < 2) continue;
			for (int n2 = 0; n2 < selectedPhases.size(); n2++)
			{
				int indexPhase = (n1 - 1) * selectedPhases.size() + n2;
				((DBS_EquilibriumPhaseFraction*)tempPhaseFraction[indexPhase])->Temperature = std::stold(string_manipulators::get_numeric_value(bufferCells[1]));
				((DBS_EquilibriumPhaseFraction*)tempPhaseFraction[indexPhase])->Value = std::stold(phaseValues[n2]);
			}

		}

		// save in database and remove from memory
		int resp = IAM_DBS::save(tempPhaseFraction);
		for (int n1 = 0; n1 < tempPhaseFraction.size(); n1++)
		{
			delete tempPhaseFraction[n1];
		}


		lua_pushstring(state, outCommand_1.c_str());
		return 1;
	}

#pragma endregion

#pragma region PixelCase
	/// <summary>
	/// Creates a new instance of AM_Project, thus it does not depend on the
	/// current open project. Be aware that loading a AM_Project might have
	/// a big overhead, use for single operations.
	/// Input Parameters: INT <IDProject> INT <IDCase>
	/// </summary>
	/// <param name="state"></param>
	/// <returns></returns>
	static int Bind_SPC_StepEquilibrium_ByProjectID(lua_State* state)
	{

		int noParameters = lua_gettop(state);
		if (noParameters < 2)
		{
			lua_pushstring(state, "usage: <INT project ID> <INT Case ID> ");
			return 1;
		}


		std::string idCase = lua_tostring(state, 2);
		std::string idProject = lua_tostring(state, 1);
		DBS_Project tempP(_dbFramework->get_database(), _openProject->get_project_ID());

		// Load the project, if id is -1, the project does not exist.
		AM_Project projectTemp(_dbFramework->get_database(), _configuration, std::stoi(idProject));
		if(projectTemp.get_project_ID() == -1)
		{
			lua_pushstring(state, "Selected IDProject does not exist!");
			return 1;
		}

		// If pixel_parameters is a null pointer, that means that either the case does not exist or
		// it corresponds to another project ID
		AM_pixel_parameters* pixel_parameters = projectTemp.get_pixelCase(std::stoi(idCase));
		if(pixel_parameters == nullptr)
		{
			lua_pushstring(state, "IDCase does not belong to the project or it does not exist!");
			return 1;
		}

		std::string outCommand_1 = runVectorCommands(API_Scripting::Script_run_stepEquilibrium(_configuration, 
																								pixel_parameters->get_EquilibriumConfiguration()->StartTemperature,
																								pixel_parameters->get_EquilibriumConfiguration()->EndTemperature,
																								projectTemp.get_selected_elements_ByName(),
																								pixel_parameters->get_composition_string(),
																								pixel_parameters->get_selected_phases_ByName() ));

		lua_pushstring(state, outCommand_1.c_str());
		return 1;
	}

	/// <summary>
	/// Based on the current openProject, runs a stepped equilibrium calculation
	/// input Parameters: INT <IDCase>
	/// </summary>
	/// <param name="state"></param>
	/// <returns></returns>
	static int Bind_SPC_StepEquilibrium(lua_State* state)
	{
		if (check_parameters(state, lua_gettop(state), 1, "usage: <INT Case ID>") != 0) return 1;
		std::vector<std::string> parameters = get_parameters(state);

		// clear buffer before starting
		std::string outClearBuffer = run_command(state, "matcalc_buffer_clear", std::vector<std::string> {"_default_"});
		
		// If pixel_parameters is a null pointer, that means that either the case does not exist or
		// it corresponds to another project ID
		AM_pixel_parameters* pixel_parameters = _openProject->get_pixelCase(std::stoi(parameters[0]));
		if (pixel_parameters == nullptr)
		{
			lua_pushstring(state, "Error: IDCase does not belong to the project or it does not exist!");
			return 1;
		}

		// Initialize case
		//TODO: When no selected elements, gives Error, catch this one
		std::string outCommand_1 = runVectorCommands(API_Scripting::Script_run_stepEquilibrium(_configuration,
			pixel_parameters->get_EquilibriumConfiguration()->StartTemperature,
			pixel_parameters->get_EquilibriumConfiguration()->EndTemperature,
			_openProject->get_selected_elements_ByName(),
			pixel_parameters->get_composition_string(),
			pixel_parameters->get_selected_phases_ByName()));

		// Get buffer content
		std::string buffer_raw = run_command(state, "matcalc_buffer_listContent");
		std::vector<std::string> bufferRowEntries = string_manipulators::split_text(buffer_raw, "\n");
		if (bufferRowEntries.size() < 2) 
		{
			std::string ErrorOut = "Error: Equilibrium calculation was not possible -> " + outCommand_1;
			lua_pushstring(state, ErrorOut.c_str());
			return 1;
		}

		// pixel parameters
		std::vector<std::string> selectedPhases = pixel_parameters->get_selected_phases_ByName();
		std::vector<int> selectedPhases_id = pixel_parameters->get_selected_phases_ByID();
		std::vector<IAM_DBS*> tempPhaseFraction((bufferRowEntries.size() - 1)* selectedPhases.size());

		// Initialize all phase fractions
		for (size_t n1 = 0; n1 < tempPhaseFraction.size(); n1++)
			tempPhaseFraction[n1] = new DBS_EquilibriumPhaseFraction(_dbFramework->get_database(), -1);

		// store buffer into 
		std::vector<std::string> BufferRows = read_matcalc_calcphase_buffer(bufferRowEntries.size() - 1, selectedPhases);
		
		// copy data to database objects
		for (int n1 = 0; n1 < BufferRows.size(); n1++)
		{
			std::vector<std::string> phaseValues = string_manipulators::split_text(BufferRows[n1], ",");

			for (int n2 = 1; n2 < phaseValues.size(); n2++)
			{
				int indexPhase = (n1) * selectedPhases.size() + (n2 - 1);
				((DBS_EquilibriumPhaseFraction*)tempPhaseFraction[indexPhase])->IDCase = pixel_parameters->get_caseID();
				((DBS_EquilibriumPhaseFraction*)tempPhaseFraction[indexPhase])->IDPhase = selectedPhases_id[n2-1];
				((DBS_EquilibriumPhaseFraction*)tempPhaseFraction[indexPhase])->Temperature = std::stold(phaseValues[0]);
				((DBS_EquilibriumPhaseFraction*)tempPhaseFraction[indexPhase])->Value = std::stold(phaseValues[n2]);
			}
		}

		// save in database and remove from memory
		int resp = IAM_DBS::save(tempPhaseFraction);
		for (int n1 = 0; n1 < tempPhaseFraction.size(); n1++)
		{
			delete tempPhaseFraction[n1];
		}
		

		lua_pushstring(state, outCommand_1.c_str());
		return 1;
	}
	static int Bind_SPC_Parallel_StepEquilibrium(lua_State* state)
	{
		// Check and get parameters
		if (check_parameters(state, lua_gettop(state), 2, "usage: <ID project> <ID Cases>") != 0) return 1;
		std::vector<std::string> parameters = get_parameters(state);

		// Initialize AM_project object, this is used for checking if cases belong to the project
		AM_Project Project(_dbFramework->get_database(),_configuration,std::stoi(parameters[0]));
		std::vector<AM_pixel_parameters*> pixel_parameters;

		// Get pointers for all cases
		std::vector<std::string> rangeIDCase = string_manipulators::split_text(parameters[1],"-");
		int start = std::stoi(rangeIDCase[0]);
		int end = std::stoi(rangeIDCase[1]);
		int range = end - start;

		for(int n1 = 0; n1 < range + 1; n1++)
		{
			pixel_parameters.push_back(Project.get_pixelCase(start + n1));
			if(pixel_parameters[n1] == nullptr)
			{
				std::string ErrorOut = "Error: Selected ID case is not part of this project!";
				lua_pushstring(state, ErrorOut.c_str());
				return 1;
			}
		}

		// Create communication to mcc for each thread
		int threadsAvail = 5;
		int splitSize = pixel_parameters.size() / threadsAvail;
		int remainder = pixel_parameters.size() % threadsAvail;
		if(remainder + splitSize > pixel_parameters.size())
		{
			threadsAvail = 1;
			splitSize = 1;
		}

		std::vector<IPC_winapi*> mcc_comms;
		for(int n1 = 0; n1 < threadsAvail; n1++)
		{
			mcc_comms.push_back(new IPC_winapi(L"C:/Program Files/MatCalc 6/mcc.exe"));
			mcc_comms[n1]->set_endflag("MC:");
		}
			

		auto funcStep = [](IPC_winapi* mccComm, std::vector<AM_pixel_parameters*> PixelList, AM_Project* projectM )
		{
			for(AM_pixel_parameters* pixel_parameters : PixelList)
			{
				std::string outCommand_1 = runVectorCommands(API_Scripting::Script_run_stepEquilibrium(_configuration,
					pixel_parameters->get_EquilibriumConfiguration()->StartTemperature,
					pixel_parameters->get_EquilibriumConfiguration()->EndTemperature,
					projectM->get_selected_elements_ByName(),
					pixel_parameters->get_composition_string(),
					pixel_parameters->get_selected_phases_ByName()), mccComm);

				// Get buffer content, check if buffer contains data 
				// before continuing
				std::string buffer_raw = matcalc_buffer_listContent(mccComm);
				std::vector<std::string> bufferRowEntries = string_manipulators::split_text(buffer_raw, "\n");
				if (bufferRowEntries.size() < 2) continue;

				// pixel parameters
				std::vector<std::string> selectedPhases = pixel_parameters->get_selected_phases_ByName();
				std::vector<int> selectedPhases_id = pixel_parameters->get_selected_phases_ByID();
				std::vector<IAM_DBS*> tempPhaseFraction((bufferRowEntries.size() - 1) * selectedPhases.size());

				// Initialize all phase fractions
				for (size_t n1 = 0; n1 < tempPhaseFraction.size(); n1++)
					tempPhaseFraction[n1] = new DBS_EquilibriumPhaseFraction(_dbFramework->get_database(), -1);

				// store buffer into 
				std::vector<std::string> BufferRows = read_matcalc_calcphase_buffer(bufferRowEntries.size() - 1, selectedPhases, mccComm);

				// copy data to database objects
				for (int n1 = 0; n1 < BufferRows.size(); n1++)
				{
					std::vector<std::string> phaseValues = string_manipulators::split_text(BufferRows[n1], ",");

					for (int n2 = 1; n2 < phaseValues.size(); n2++)
					{
						int indexPhase = (n1)*selectedPhases.size() + (n2 - 1);
						((DBS_EquilibriumPhaseFraction*)tempPhaseFraction[indexPhase])->IDCase = pixel_parameters->get_caseID();
						((DBS_EquilibriumPhaseFraction*)tempPhaseFraction[indexPhase])->IDPhase = selectedPhases_id[n2 - 1];
						((DBS_EquilibriumPhaseFraction*)tempPhaseFraction[indexPhase])->Temperature = std::stold(phaseValues[0]);
						((DBS_EquilibriumPhaseFraction*)tempPhaseFraction[indexPhase])->Value = std::stold(phaseValues[n2]);
					}
				}

				// save in database and remove from memory
				_mutex.lock();
				int resp = IAM_DBS::save(tempPhaseFraction);
				for (int n1 = 0; n1 < tempPhaseFraction.size(); n1++)
				{
					delete tempPhaseFraction[n1];
				}
				_mutex.unlock();

				mccComm->send_command("exit\r\n");
				delete mccComm;

				mccComm = new IPC_winapi(L"C:/Program Files/MatCalc 6/mcc.exe");
				mccComm->set_endflag("MC:");
			}
		};

		int Index = 0;
		std::vector<std::thread> threadList;
		for(int n1 = 0; n1 < threadsAvail; n1++)
		{
			std::vector<AM_pixel_parameters*> tempVector(pixel_parameters.begin() + n1* splitSize, pixel_parameters.begin() + (n1 + 1)*splitSize);
			threadList.push_back(std::thread(funcStep, mcc_comms[n1], tempVector, & Project));
			Index += splitSize;
		}
			
		for (int n1 = 0; n1 < threadsAvail; n1++)
		{
			threadList[n1].join();
		}

		for (int n1 = 0; n1 < threadsAvail; n1++)
		{
			mcc_comms[n1]->send_command("exit\r\n");
			delete mcc_comms[n1];
		}
		mcc_comms.clear();

		std::string outmsg = "OK";
		lua_pushstring(state, outmsg.c_str());
		return 1;
	}

	/// <summary>
	/// Based on the current openProject, runs a stepped equilibrium calculation
	/// input Parameters: INT <IDCase>
	/// </summary>
	/// <param name="state"></param>
	/// <returns></returns>
	static int Bind_SPC_StepScheil(lua_State* state)
	{
		if (check_parameters(state, lua_gettop(state), 1, "usage: <INT Case ID>") != 0) return 1;
		std::vector<std::string> parameters = get_parameters(state);

		// clear buffer before starting
		std::string outClearBuffer = run_command(state, "matcalc_buffer_clear", std::vector<std::string> {"_default_"});

		// If pixel_parameters is a null pointer, that means that either the case does not exist or
		// it corresponds to another project ID
		AM_pixel_parameters* pixel_parameters = _openProject->get_pixelCase(std::stoi(parameters[0]));
		if (pixel_parameters == nullptr)
		{
			lua_pushstring(state, "Error: IDCase does not belong to the project or it does not exist!");
			return 1;
		}

		// Initialize case
		std::string outCommand_1 = runVectorCommands(API_Scripting::Script_run_stepScheilEquilibrium(_configuration,
			pixel_parameters->get_ScheilConfiguration()->StartTemperature,
			pixel_parameters->get_ScheilConfiguration()->EndTemperature,
			pixel_parameters->get_scheil_config_StepSize(),
			_openProject->get_selected_elements_ByName(),
			pixel_parameters->get_composition_string(),
			pixel_parameters->get_selected_phases_ByName()));

		// Get buffer content
		std::string buffer_raw = run_command(state, "matcalc_buffer_listContent");
		std::vector<std::string> bufferRowEntries = string_manipulators::split_text(buffer_raw, "\n");
		if (bufferRowEntries.size() < 2)
		{
			std::string ErrorOut = "Error: Equilibrium calculation was not possible -> " + outCommand_1;
			lua_pushstring(state, ErrorOut.c_str());
			return 1;
		}

		// pixel parameters
		std::vector<std::string> selectedPhases = pixel_parameters->get_selected_phases_ByName();
		std::vector<int> selectedPhases_id = pixel_parameters->get_selected_phases_ByID();
		std::vector<IAM_DBS*> tempPhaseFraction((bufferRowEntries.size() - 1) * selectedPhases.size());
		std::vector<IAM_DBS*> tempPhaseCumulativeFraction((bufferRowEntries.size() - 1) * selectedPhases.size());

		for (size_t n1 = 0; n1 < tempPhaseFraction.size(); n1++)
		{
			tempPhaseFraction[n1] = new DBS_ScheilPhaseFraction(_dbFramework->get_database(), -1);
			tempPhaseCumulativeFraction[n1] = new DBS_ScheilCumulativeFraction(_dbFramework->get_database(), -1);
		}

		// store buffer into 
		std::vector<std::string> BufferRows = read_matcalc_calcphase_buffer(bufferRowEntries.size() - 1, selectedPhases);

		// copy data to database objects
		for (int n1 = 0; n1 < BufferRows.size(); n1++)
		{
			std::vector<std::string> phaseValues = string_manipulators::split_text(BufferRows[n1], ",");

			for (int n2 = 1; n2 < phaseValues.size(); n2++)
			{
				int indexPhase = (n1)*selectedPhases.size() + (n2 - 1);
				((DBS_EquilibriumPhaseFraction*)tempPhaseFraction[indexPhase])->IDCase = pixel_parameters->get_caseID();
				((DBS_EquilibriumPhaseFraction*)tempPhaseFraction[indexPhase])->IDPhase = selectedPhases_id[n2 - 1];
				((DBS_EquilibriumPhaseFraction*)tempPhaseFraction[indexPhase])->Temperature = std::stold(phaseValues[0]);
				((DBS_EquilibriumPhaseFraction*)tempPhaseFraction[indexPhase])->Value = std::stold(phaseValues[n2]);
			}
		}

		// cumulative fraction, save as table
		for (int n1 = 1; n1 < tempPhaseFraction.size(); n1++)
		{
			for (int n2 = 0; n2 < selectedPhases.size(); n2++)
			{
				((DBS_ScheilCumulativeFraction*)tempPhaseCumulativeFraction[n1])->Value = ((DBS_ScheilPhaseFraction*)tempPhaseFraction[n1])->Value + 
																						  ((DBS_ScheilCumulativeFraction*)tempPhaseCumulativeFraction[n1-1])->Value;
			}
		}

		// Save all entries
		tempPhaseCumulativeFraction.insert(tempPhaseCumulativeFraction.end(), tempPhaseFraction.begin(), tempPhaseFraction.end());
		int resp = IAM_DBS::save(tempPhaseCumulativeFraction);
		for (int n1 = 0; n1 < tempPhaseFraction.size(); n1++)
		{
			delete tempPhaseFraction[n1];
			delete tempPhaseCumulativeFraction[n1];
		}

		lua_pushstring(state, outCommand_1.c_str());
		return 1;
	}

	/// <summary>
	/// This will run all pending cases in current project
	/// </summary>
	/// <param name="state"></param>
	/// <returns></returns>
	static int Bind_SPC_run_cases(lua_State* state)
	{
		std::vector<std::string> parameters;
		if (check_global_using_openProject(state, lua_gettop(state), 0,
			" No input required ",
			parameters) != 0) return 1;
		
		// we have to run all configurations
		std::vector<std::string> out;
		for (int n1 = 0; n1 < _openProject->get_singlePixel_Cases().size(); n1++)
		{
			// TODO: do this a nicer way if you get the chance, I noticed you don't sleep
			// and now you are talking to a machine.
			// do step equilibrium
			lua_getglobal(state, "pixelcase_stepEquilibrium");
			lua_pushstring(state, std::to_string(_openProject->get_singlePixel_Cases()[n1]->get_caseID()).c_str());
			lua_pcall(state, 1, 1, 0);
			out.push_back(lua_tostring(state,-1));
			lua_pop(state, 1);

			lua_getglobal(state, "pixelcase_stepScheil");
			lua_pushstring(state, std::to_string(_openProject->get_singlePixel_Cases()[n1]->get_caseID()).c_str());
			lua_pcall(state, 1, 1, 0);
			out.push_back(lua_tostring(state, -1));
			lua_pop(state, 1);
		}

		lua_pushstring(state, "OK");
		return 1;
	}
#pragma endregion

#pragma endregion

#pragma region helpers
	/// <summary>
	/// runs commands contained by a vector, one by one.
	/// </summary>
	/// <param name="parameter"></param>
	/// <returns></returns>
	static std::string runVectorCommands(std::vector<std::string> parameter);
	static std::string runVectorCommands(std::vector<std::string> parameter, IPC_winapi* mcc_comm);

	static std::vector<std::string> read_matcalc_calcphase_buffer(int BufferSize, std::vector<std::string>& selectedPhases, IPC_winapi* mccComm = nullptr)
	{
		// store buffer into 
		std::string formatVariable = string_manipulators::get_string_format_numeric_generic(selectedPhases.size(), "%g", ",");
		std::string FORMATString = API_Scripting::script_format_variable_string("TempData", formatVariable, API_Scripting::script_get_phase_equilibrium_variable_name(selectedPhases));
		std::string scriptFile = API_Scripting::Buffer_to_variable(std::to_string(BufferSize), FORMATString, "TempData", _configuration);
		
		// run script
		std::string run_;
		if(mccComm == nullptr)
		{
			run_ = _api->APIcommand(API_Scripting::script_runScript(scriptFile));
		}
		else
		{
			run_ = _api->APIcommand(API_Scripting::script_runScript(scriptFile), mccComm);
		}

		std::filesystem::remove(scriptFile);

		std::vector<std::string> BufferRows = string_manipulators::read_file_to_end(scriptFile.substr(0, scriptFile.size() - 4) + ".Framework");
		std::filesystem::remove(scriptFile.substr(0, scriptFile.size() - 4) + ".Framework");
		return BufferRows;
	}
#pragma endregion

};
/** @}*/
