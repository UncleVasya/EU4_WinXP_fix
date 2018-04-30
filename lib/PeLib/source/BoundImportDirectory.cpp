/*
* BoundImportDirectory.cpp - Part of the PeLib library.
*
* Copyright (c) 2004 - 2005 Sebastian Porst (webmaster@the-interweb.com)
* All rights reserved.
*
* This software is licensed under the zlib/libpng License.
* For more details see http://www.opensource.org/licenses/zlib-license.php
* or the license information file (license.htm) in the root directory 
* of PeLib.
*/

#include "PeLibInc.h"
#include "BoundImportDirectory.h"
#include <numeric>
#include <set>
#include <map>

namespace PeLib
{
	/**
	* Adds another bound import to the BoundImport directory.
	* @param strModuleName Name of the PE file which will be imported.
	* @param dwTds Value of the TimeDateStamp of the bound import field.
	* @param wOmn Value of the OffsetModuleName of the bound import field.
	* @param wWfr Value of the NumberOfModuleForwarderRefs of the bound import field.
	**/
	int BoundImportDirectory::addBoundImport(const std::string& strModuleName, dword dwTds, word wOmn, word wWfr)
	{
		for (unsigned int i=0;i<m_vIbd.size();i++)
		{
			if (isEqualNc(strModuleName, m_vIbd[i].strModuleName))
			{
				return ERROR_DUPLICATE_ENTRY;
			}
		}
		
		PELIB_IMAGE_BOUND_IMPORT_DESCRIPTOR ibidCurrent;
		ibidCurrent.TimeDateStamp = dwTds;
		ibidCurrent.OffsetModuleName = wOmn;
		ibidCurrent.NumberOfModuleForwarderRefs = wWfr;
		PELIB_IMAGE_BOUND_DIRECTORY ibdCurrent;
		ibdCurrent.ibdDescriptor = ibidCurrent;
		ibdCurrent.strModuleName = strModuleName;
		m_vIbd.push_back(ibdCurrent);
		
		return NO_ERROR;
	}

	/**
	* Searches for the first instance of a module with the given modulename.
	* @param strModuleName The name of a module.
	* @return The id of the module.
	**/
	int BoundImportDirectory::getModuleIndex(const std::string& strModuleName) const
	{
		std::vector<PELIB_IMAGE_BOUND_DIRECTORY>::const_iterator Iter = std::find_if(m_vIbd.begin(), m_vIbd.end(), std::bind2nd(std::mem_fun_ref(&PELIB_IMAGE_BOUND_DIRECTORY::equal), strModuleName));
		
		if (Iter == m_vIbd.end())
		{
			return ERROR_ENTRY_NOT_FOUND;
		}
		
		return static_cast<int>(std::distance(m_vIbd.begin(), Iter));
	}

	/**
	* @return Number of files in the current BoundImport directory.
	**/
	unsigned int BoundImportDirectory::calcNumberOfModules() const
	{
		return static_cast<unsigned int>(m_vIbd.size());
	}

	int BoundImportDirectory::read(InputBuffer& inpBuffer, unsigned char* data, unsigned int dwSize)
	{
		std::vector<PELIB_IMAGE_BOUND_DIRECTORY> currentDirectory;
		
		do
		{
			PELIB_IMAGE_BOUND_DIRECTORY ibdCurrent;
			
			inpBuffer >> ibdCurrent.ibdDescriptor.TimeDateStamp;
			inpBuffer >> ibdCurrent.ibdDescriptor.OffsetModuleName;
			inpBuffer >> ibdCurrent.ibdDescriptor.NumberOfModuleForwarderRefs;

			if (ibdCurrent.ibdDescriptor.TimeDateStamp == 0 && ibdCurrent.ibdDescriptor.OffsetModuleName == 0 && ibdCurrent.ibdDescriptor.NumberOfModuleForwarderRefs == 0) break;
			
			for (int i=0;i<ibdCurrent.ibdDescriptor.NumberOfModuleForwarderRefs;i++)
			{
				PELIB_IMAGE_BOUND_DIRECTORY currentForwarder;
				
				inpBuffer >> currentForwarder.ibdDescriptor.TimeDateStamp;
				inpBuffer >> currentForwarder.ibdDescriptor.OffsetModuleName;
				inpBuffer >> currentForwarder.ibdDescriptor.NumberOfModuleForwarderRefs;
				
				ibdCurrent.moduleForwarders.push_back(currentForwarder);
			}
			
			currentDirectory.push_back(ibdCurrent);
			ibdCurrent.moduleForwarders.clear();
		} while (true);
		
		for (unsigned int i=0;i<currentDirectory.size();i++)
		{
			dword wOmn = currentDirectory[i].ibdDescriptor.OffsetModuleName;
			if (wOmn > dwSize)
			{
				return ERROR_INVALID_FILE;
			}
			
			currentDirectory[i].strModuleName = "";
			for (int k=0;data[wOmn + k] != 0 && k + wOmn < dwSize;k++)
			{
				currentDirectory[i].strModuleName += data[wOmn + k];
			}
			
			for (unsigned int j=0;j<currentDirectory[i].moduleForwarders.size();j++)
			{
				dword wOmn = currentDirectory[i].moduleForwarders[j].ibdDescriptor.OffsetModuleName;
				
				if (wOmn > dwSize)
				{
					return ERROR_INVALID_FILE;
				}
				
//				m_vIbd[i].moduleForwarders[j].strModuleName.assign((char*)(&vBimpDir[wOmn]));
				currentDirectory[i].moduleForwarders[j].strModuleName = "";
				for (int k=0;data[wOmn + k] != 0 && k + wOmn < dwSize;k++)
				{
					currentDirectory[i].moduleForwarders[j].strModuleName += data[wOmn + k];
				}
			}
		}
		
		std::swap(m_vIbd, currentDirectory);
		
		return NO_ERROR;
	}

	/**
	* Reads the BoundImport directory from a PE file.
	* @param strModuleName The name of the PE file from which the BoundImport directory is read.
	* @param dwOffset The file offset where the BoundImport directory can be found (see #PeFile::PeHeader::getIDBoundImportRVA).
	* @param dwSize The size of the BoundImport directory (see #PeFile::PeHeader::getIDBoundImportSize).
	**/
	int BoundImportDirectory::read(const std::string& strModuleName, dword dwOffset, unsigned int uiSize)
	{
		std::ifstream ifFile(strModuleName.c_str(), std::ios::binary);

		if (!ifFile)
		{
			return ERROR_OPENING_FILE;
		}
		
		if (fileSize(ifFile) < dwOffset + uiSize)
		{
			return ERROR_INVALID_FILE;
		}
		
		ifFile.seekg(dwOffset, std::ios::beg);
		
		std::vector<unsigned char> vBimpDir(uiSize);
		ifFile.read(reinterpret_cast<char*>(&vBimpDir[0]), uiSize);

		InputBuffer inpBuffer(vBimpDir);
		
		return read(inpBuffer, &vBimpDir[0], uiSize);
 	}
 	
 	int BoundImportDirectory::read(unsigned char* pcBuffer, unsigned int uiSize)
 	{
		std::vector<unsigned char> vBimpDir(pcBuffer, pcBuffer + uiSize);
		InputBuffer inpBuffer(vBimpDir);
		
		return read(inpBuffer, &vBimpDir[0], uiSize);
 	}
 	
 	unsigned int BoundImportDirectory::totalModules() const
 	{
 		unsigned int modules = static_cast<unsigned int>(m_vIbd.size());
 		
 		for (unsigned int i=0;i<m_vIbd.size();i++)
 		{
 			modules += static_cast<unsigned int>(m_vIbd[i].moduleForwarders.size());
 		}
 		
 		return modules;
 	}

	/**
	* Rebuilds the BoundImport directory. The rebuilded BoundImport directory can then be
	* written back to a PE file.
	* @param vBuffer Buffer where the rebuilt BoundImport directory will be stored.
	* @param fMakeValid If this flag is true a valid directory will be produced.
	**/
	void BoundImportDirectory::rebuild(std::vector<byte>& vBuffer, bool fMakeValid) const
	{
		std::map<std::string, word> filename_offsets;
		
		OutputBuffer obBuffer(vBuffer);
		
		word ulNameOffset = static_cast<word>((totalModules() + 1) * PELIB_IMAGE_BOUND_IMPORT_DESCRIPTOR::size());
		
		for (unsigned int i=0;i<m_vIbd.size();i++)
		{
			obBuffer << m_vIbd[i].ibdDescriptor.TimeDateStamp;

			// Recalculate the offsets if a valid directory is wanted.
			if (fMakeValid)
			{
				if (filename_offsets.find(m_vIbd[i].strModuleName) == filename_offsets.end())
				{
					filename_offsets[m_vIbd[i].strModuleName] = ulNameOffset;
					obBuffer << ulNameOffset;
					ulNameOffset += static_cast<word>(m_vIbd[i].strModuleName.size() + 1);
				}
				else
				{
					obBuffer << filename_offsets[m_vIbd[i].strModuleName];
				}
			}
			else // Otherwise just copy the old values into the buffer.
			{
				obBuffer << m_vIbd[i].ibdDescriptor.OffsetModuleName;
			}

			obBuffer << m_vIbd[i].ibdDescriptor.NumberOfModuleForwarderRefs;
			
			for (int j=0;j<calcNumberOfModuleForwarderRefs(i);j++)
			{
				obBuffer << m_vIbd[i].moduleForwarders[j].ibdDescriptor.TimeDateStamp;
				
				if (fMakeValid)
				{
					if (filename_offsets.find(m_vIbd[i].strModuleName) == filename_offsets.end())
					{
						filename_offsets[m_vIbd[i].moduleForwarders[j].strModuleName] = ulNameOffset;
						obBuffer << ulNameOffset;
						ulNameOffset += static_cast<word>(m_vIbd[i].moduleForwarders[j].strModuleName.size() + 1);
					}
					else
					{
						obBuffer << filename_offsets[m_vIbd[i].moduleForwarders[j].strModuleName];
					}
				}
				else // Otherwise just copy the old values into the buffer.
				{
					obBuffer << m_vIbd[i].moduleForwarders[j].ibdDescriptor.OffsetModuleName;
				}

				obBuffer << m_vIbd[i].moduleForwarders[j].ibdDescriptor.NumberOfModuleForwarderRefs;
			}
		}
		
		obBuffer << static_cast<dword>(0);
		obBuffer << static_cast<word>(0);
		obBuffer << static_cast<word>(0);

		for (unsigned int i=0;i<m_vIbd.size();i++)
		{
			if (filename_offsets.find(m_vIbd[i].strModuleName) != filename_offsets.end())
			{
				obBuffer.add(getModuleName(i).c_str(), static_cast<unsigned long>(getModuleName(i).size() + 1));
				filename_offsets.erase(m_vIbd[i].strModuleName);
			}
			
			for (int j=0;j<calcNumberOfModuleForwarderRefs(i);j++)
			{
				if (filename_offsets.find(getModuleName(i, j)) != filename_offsets.end())
				{
					obBuffer.add(getModuleName(i, j).c_str(), static_cast<unsigned long>(getModuleName(i, j).size() + 1));
					filename_offsets.erase(getModuleName(i, j));
				}
			}
		}
	}

	/**
	* Removes all bound import files.
	**/
	void BoundImportDirectory::clear()
	{
		m_vIbd.clear();
	}

	/**
	* Removes a field specified by the parameter filename from the BoundImport directory.
	* @param strModuleName Name of the file whose field will be removed from the BoundImport directory.
	**/
	void BoundImportDirectory::removeBoundImport(const std::string& strModuleName)
	{
		m_vIbd.erase(std::remove_if(m_vIbd.begin(), m_vIbd.end(), std::bind2nd(std::mem_fun_ref(&PELIB_IMAGE_BOUND_DIRECTORY::equal), strModuleName)), m_vIbd.end());
	}
	
	/**
	* Returns the size of the rebuilt BoundImportDirectory.
	* @return Size of the rebuilt BoundImportDirectory.
	**/
	unsigned int BoundImportDirectory::size() const
	{
		unsigned int size = PELIB_IMAGE_BOUND_IMPORT_DESCRIPTOR::size();
		
		std::set<std::string> filenames;
		
		for (unsigned int i = 0; i < m_vIbd.size(); i++)
		{
			filenames.insert(m_vIbd[i].strModuleName);
			
			size += PELIB_IMAGE_BOUND_IMPORT_DESCRIPTOR::size();
			
			for (unsigned int j = 0; j < m_vIbd[i].moduleForwarders.size(); j++)
			{
				filenames.insert(m_vIbd[i].moduleForwarders[j].strModuleName);
				
				size += PELIB_IMAGE_BOUND_IMPORT_DESCRIPTOR::size();
			}
		}
		
		for (std::set<std::string>::iterator iter = filenames.begin(); iter != filenames.end(); ++iter)
		{
			size += static_cast<unsigned int>(iter->size()) + 1;
		}
		
		return size;
	}

	/**
	* @param strFilename Name of the file.
	* @param dwOffset File offset the bound importdirectory will be written to.
	* @param fMakeValid If this flag is true a valid directory will be produced.
	**/
	int BoundImportDirectory::write(const std::string& strFilename, dword dwOffset,  bool fMakeValid) const
	{
		std::fstream ofFile(strFilename.c_str(), std::ios_base::in);
		
		if (!ofFile)
		{
			ofFile.clear();
			ofFile.open(strFilename.c_str(), std::ios_base::out | std::ios_base::binary);
		}
		else
		{
			ofFile.close();
			ofFile.open(strFilename.c_str(), std::ios_base::in | std::ios_base::out | std::ios_base::binary);
		}

		if (!ofFile)
		{
			return ERROR_OPENING_FILE;
		}

		ofFile.seekp(dwOffset, std::ios::beg);

		std::vector<unsigned char> vBuffer;
		rebuild(vBuffer, fMakeValid);

		ofFile.write(reinterpret_cast<const char*>(&vBuffer[0]), static_cast<std::streamsize>(vBuffer.size()));

		ofFile.close();
		
		return NO_ERROR;
	}

	/**
	* Retrieves the value of the TimeDateStamp value of a bound import field.
	* @param dwBidnr Number of the bound import field.
	* @return Value of the TimeDateStamp of the bound import field.
	**/
	dword BoundImportDirectory::getTimeDateStamp(dword dwBidnr) const
	{
		return m_vIbd[dwBidnr].ibdDescriptor.TimeDateStamp;
	}
	
	/**
	* Retrieves the value of the OffsetModuleName value of a bound import field. 
	* @param dwBidnr Number of the bound import field.
	* @return Value of the OffsetModuleName of the bound import field.
	**/
	word BoundImportDirectory::getOffsetModuleName(dword dwBidnr) const
	{
		return m_vIbd[dwBidnr].ibdDescriptor.OffsetModuleName;
	}
	
	/**
	* Retrieves the value of the NumberOfModuleForwarderRefs value of a bound import field.
	* @param dwBidnr Number of the bound import field.
	* @return Value of the NumberOfModuleForwarderRefs of the bound import field.
	**/
	word BoundImportDirectory::getNumberOfModuleForwarderRefs(dword dwBidnr) const
	{
		return m_vIbd[dwBidnr].ibdDescriptor.NumberOfModuleForwarderRefs;
	}
	
	/**
	* Retrieves the value of the ModuleName value of a bound import field.
	* @param dwBidnr Number of the bound import field.
	* @return Value of the ModuleName of the bound import field.
	**/
	std::string BoundImportDirectory::getModuleName(dword dwBidnr) const
	{
		return m_vIbd[dwBidnr].strModuleName;
	}

	/**
	* Changes the TimeDateStamp value of an existing bound import field.
	* @param dwBidnr Number of the bound import field which will be changed.
	* @param dwTds New value of the TimeDateStamp of the bound import field.
	**/
	void BoundImportDirectory::setTimeDateStamp(dword dwBidnr, dword dwTds)
	{
		m_vIbd[dwBidnr].ibdDescriptor.TimeDateStamp = dwTds;
	}

	/**
	* Changes the OffsetModuleName value of an existing bound import field.
	* @param dwBidnr Number of the bound import field which will be changed.
	* @param wOmn New value of the OffsetModuleName of the bound import field.
	**/
	void BoundImportDirectory::setOffsetModuleName(dword dwBidnr, word wOmn)
	{
		m_vIbd[dwBidnr].ibdDescriptor.OffsetModuleName = wOmn;
	}
	
	/**
	* Changes the NumberOfModuleForwarderRefs value of an existing bound import field.
	* @param dwBidnr Number of the bound import field which will be changed.
	* @param wMfr New value of the NumberOfModuleForwarderRefs of the bound import field.
	**/
	void BoundImportDirectory::setNumberOfModuleForwarderRefs(dword dwBidnr, word wMfr)
	{
		m_vIbd[dwBidnr].ibdDescriptor.NumberOfModuleForwarderRefs = wMfr;
	}
	
	/**
	* Changes the ModuleName value of an existing bound import field.
	* @param dwBidnr Number of the bound import field which will be changed.
	* @param strModuleName New value of the ModuleName of the bound import field.
	**/
	void BoundImportDirectory::setModuleName(dword dwBidnr, const std::string& strModuleName)
	{
		m_vIbd[dwBidnr].strModuleName = strModuleName;
	}
	
	dword BoundImportDirectory::getTimeDateStamp(dword dwBidnr, dword forwardedModule) const
	{
		return m_vIbd[dwBidnr].moduleForwarders[forwardedModule].ibdDescriptor.TimeDateStamp;
	}
	
	word BoundImportDirectory::getOffsetModuleName(dword dwBidnr, dword forwardedModule) const
	{
		return m_vIbd[dwBidnr].moduleForwarders[forwardedModule].ibdDescriptor.OffsetModuleName;
	}
	
	word BoundImportDirectory::getNumberOfModuleForwarderRefs(dword dwBidnr, dword forwardedModule) const
	{
		return m_vIbd[dwBidnr].moduleForwarders[forwardedModule].ibdDescriptor.NumberOfModuleForwarderRefs;
	}
	
	std::string BoundImportDirectory::getModuleName(dword dwBidnr, dword forwardedModule) const
	{
		return m_vIbd[dwBidnr].moduleForwarders[forwardedModule].strModuleName;
	}
	
	void BoundImportDirectory::setTimeDateStamp(dword dwBidnr, dword forwardedModule, dword dwTds)
	{
		m_vIbd[dwBidnr].moduleForwarders[forwardedModule].ibdDescriptor.TimeDateStamp = dwTds;
	}
	
	void BoundImportDirectory::setOffsetModuleName(dword dwBidnr, dword forwardedModule, word wOmn)
	{
		m_vIbd[dwBidnr].moduleForwarders[forwardedModule].ibdDescriptor.OffsetModuleName = wOmn;
	}
	
	void BoundImportDirectory::setNumberOfModuleForwarderRefs(dword dwBidnr, dword forwardedModule, word wMfr)
	{
		m_vIbd[dwBidnr].moduleForwarders[forwardedModule].ibdDescriptor.NumberOfModuleForwarderRefs = wMfr;
	}

	void BoundImportDirectory::setModuleName(dword dwBidnr, dword forwardedModule, const std::string& strModuleName)
	{
		m_vIbd[dwBidnr].moduleForwarders[forwardedModule].strModuleName = strModuleName;
	}
	
	word BoundImportDirectory::calcNumberOfModuleForwarderRefs(dword dwBidnr) const
	{
		return static_cast<word>(m_vIbd[dwBidnr].moduleForwarders.size());
	}
	
	void BoundImportDirectory::addForwardedModule(dword dwBidnr, const std::string& name, dword timeStamp, word offsetModuleName, word forwardedModules)
	{
		// XXX: Maybe test if there are already 0xFFFF forwarded modules.
		// XXX: Check for duplicate entries. Is it also necessary to check
		//      non-forwarded entries and forwarded entries in other non-forwarded
		//      entries?
		// XXX: Can forwarders forward recursively?
		
		PELIB_IMAGE_BOUND_DIRECTORY ibdCurrent;
		ibdCurrent.strModuleName = name;
		ibdCurrent.ibdDescriptor.TimeDateStamp = timeStamp;
		ibdCurrent.ibdDescriptor.OffsetModuleName = offsetModuleName;
		ibdCurrent.ibdDescriptor.NumberOfModuleForwarderRefs = forwardedModules;
		
		m_vIbd[dwBidnr].moduleForwarders.push_back(ibdCurrent);
	}
	
	void BoundImportDirectory::removeForwardedModule(dword dwBidnr, word forwardedModule)
	{
		m_vIbd[dwBidnr].moduleForwarders.erase(m_vIbd[dwBidnr].moduleForwarders.begin() + forwardedModule);
	}
}
