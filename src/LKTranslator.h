#ifndef KCLIB_LKTRANSLATOR_H
#define KCLIB_LKTRANSLATOR_H

#include <functional>
#include <map>
#include <deque>
#include <set>
#include <string>

/**
 * Handles translation of lines via a crc32 lookup table.
 */
class LKTranslator
{
public:
	/**
	 * A possible loading state for the translation data.
	 */
	enum LoadStatus {
		LoadStatusNotLoaded,	///< No attempt to load the data has been made.
		LoadStatusLoading,		///< Data is currently loading.
		LoadStatusLoaded,		///< Data is fully loaded.
		LoadStatusError			///< Data loading failed with an error.
	};
	
	/**
	 * Translates the given line, according to the current translation data.
	 * 
	 * @see loadStatus
	 * @see reportCallback
	 * 
	 * @param line The line to be translated. Encoding doesn't really matter,
	 *        as translation is done with binary crc32 sums.
	 * @param lastPathComponent The last component of the path to the endpoint
	 *        where the string was encountered.
	 * @param jsonKey The key for the line in the document it was encountered,
	 *        provided for translation context and blacklisting.
	 * 
	 * @returns The translated string, or the original one if there is no
	 *          translation available.
	 */
	std::string translate(std::string line, std::string lastPathComponent = "", std::string jsonKey = "");
	
	/**
	 * Current load status for the translation data.
	 * 
	 * This should be set externally by your program, as kclib has no
	 * networking facilities of its own.
	 */
	LoadStatus loadStatus = LoadStatusNotLoaded;
	
	/**
	 * Current translation data.
	 * Keys are crc32 sums of untranslated lines, and values are the lines'
	 * translated equivalents.
	 * 
	 * This should be set externally by your program, as kclib has no
	 * networking facilities of its own.
	 */
	std::map<uint32_t, std::string> translationData;
	
	/**
	 * Current load status for the blacklist.
	 * 
	 * @see loadStatus
	 */
	LoadStatus blacklistLoadStatus = LoadStatusNotLoaded;
	
	/**
	 * Current submission blacklist.
	 * 
	 * Keys are final path components, values are JSON keys. Lines that match
	 * are considered untranslatable, and will not be submitted to the server.
	 * 
	 * Either key or value may be "*"; matching either any path component, or
	 * any JSON key. "*": "*" is disallowed though, it'd completely break
	 * submission
	 */
	std::map<std::string, std::set<std::string>> blacklist;
	
	/**
	 * Callback for reporting untranslated lines.
	 */
	std::function<void(std::string line, std::string lastPathComponent, std::string key)> reportCallback;
	
protected:
	/**
	 * Handles untranslated lines.
	 * 
	 * If the stars align, this line will actually reach the report callback.
	 */
	void handleUntranslatedLine(std::string line, std::string lastPathComponent, std::string jsonKey);
	
	/**
	 * An entry for the report backlog.
	 */
	struct BacklogEntry {
		std::string line, lastPathComponent, jsonKey;
	};
	
	/**
	 * The report backlog.
	 * 
	 * When lines are received before we have a report blacklist, we store them
	 * here until it arrives, then go through it.
	 */
	std::deque<BacklogEntry> backlog;
};

#endif
