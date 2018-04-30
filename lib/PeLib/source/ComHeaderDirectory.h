/*
* ComHeaderDirectory.h - Part of the PeLib library.
*
* Copyright (c) 2004 - 2005 Sebastian Porst (webmaster@the-interweb.com)
* All rights reserved.
*
* This software is licensed under the zlib/libpng License.
* For more details see http://www.opensource.org/licenses/zlib-license.php
* or the license information file (license.htm) in the root directory 
* of PeLib.
*/

#ifndef COMHEADERDIRECTORY_H
#define COMHEADERDIRECTORY_H

namespace PeLib
{
	/// Class that handles the COM+ descriptor directory.
	/**
	* This class handles the COM+ Descriptor directory which was added to PE files
	* which work with the .NET runtime modules.
	**/
	class ComHeaderDirectory
	{
		private:
		  PELIB_IMAGE_COR20_HEADER m_ichComHeader; ///< The COM+ descriptor.
		  
		  void read(InputBuffer& inputbuffer);

		public:
		  /// Read a file's COM+ runtime descriptor directory.
		  int read(const std::string& strFilename, unsigned int uiOffset, unsigned int uiSize); // EXPORT
		  int read(unsigned char* buffer, unsigned int buffersize); // EXPORT
		  /// Rebuild the COM+ descriptor.
		  void rebuild(std::vector<byte>& vBuffer) const; // EXPORT
		  /// Returns the size of the current COM+ descriptor.
		  unsigned int size() const; // EXPORT
		  /// Writes the current COM+ descriptor directory to a file.
		  int write(const std::string& strFilename, unsigned int dwOffset) const; // EXPORT

		  /// Get the COM+ descriptor's SizeOfHeader (cb) value.
		  dword getSizeOfHeader() const; // EXPORT
		  /// Get the COM+ descriptor's MajorRuntimeVersion value.
		  word getMajorRuntimeVersion() const; // EXPORT
		  /// Get the COM+ descriptor's MinorRuntimeVersion value.
		  word getMinorRuntimeVersion() const; // EXPORT
		  /// Get the COM+ descriptor's MetaData (Virtual Address) value.
		  dword getMetaDataVa() const; // EXPORT
		  /// Get the COM+ descriptor's MetaData (Size) value.
		  dword getMetaDataSize() const; // EXPORT
		  /// Get the COM+ descriptor's Flags value.
		  dword getFlags() const; // EXPORT
		  /// Get the COM+ descriptor's EntryPointToken value.
		  dword getEntryPointToken() const; // EXPORT
		  /// Get the COM+ descriptor's Resources (Virtual Address) value.
		  dword getResourcesVa() const; // EXPORT
		  /// Get the COM+ descriptor's Resources (Size) value.
		  dword getResourcesSize(); // EXPORT
		  /// Get the COM+ descriptor's StrongNameSignature (Virtual Address) value.
		  dword getStrongNameSignatureVa() const; // EXPORT
		  /// Get the COM+ descriptor's StrongNameSignature (Size) value.
		  dword getStrongNameSignagureSize() const; // EXPORT
		  /// Get the COM+ descriptor's CodeManagerTable (Virtual Address) value.
		  dword getCodeManagerTableVa() const; // EXPORT
		  /// Get the COM+ descriptor's CodeManagerTable (Size) value.
		  dword getCodeManagerTableSize() const; // EXPORT
		  /// Get the COM+ descriptor's VTableFixup (Virtual Address) value.
		  dword getVTableFixupsVa() const; // EXPORT
		  /// Get the COM+ descriptor's VTableFixup (Size) value.
		  dword getVTableFixupsSize() const; // EXPORT
		  /// Get the COM+ descriptor's ExportAddressTable (Virtual Address) value.
		  dword getExportAddressTableJumpsVa() const; // EXPORT
		  /// Get the COM+ descriptor's ExportAddressTable (Size) value.
		  dword getExportAddressTableJumpsSize() const; // EXPORT
		  /// Get the COM+ descriptor's ManagedNativeHeader (Virtual Address) value.
		  dword getManagedNativeHeaderVa() const; // EXPORT
		  /// Get the COM+ descriptor's ManagedNativeHeader (Size) value.
		  dword getManagedNativeHeaderSize() const; // EXPORT

		  /// Change the COM+ descriptor's SizeOfHeader (cb) value.
		  void setSizeOfHeader(dword dwValue); // EXPORT
		  /// Change the COM+ descriptor's MajorRuntimeVersion value.
		  void setMajorRuntimeVersion(word wValue); // EXPORT
		  /// Change the COM+ descriptor's MinorRuntimeVersion value.
		  void setMinorRuntimeVersion(word wValue); // EXPORT
		  /// Change the COM+ descriptor's MetaData (VirtualAddress) value.
		  void setMetaDataVa(dword dwValue); // EXPORT
		  /// Change the COM+ descriptor's MetaData (Size) value.
		  void setMetaDataSize(dword dwValue); // EXPORT
		  /// Change the COM+ descriptor's Flags value.
		  void setFlags(dword dwValue); // EXPORT
		  /// Change the COM+ descriptor's EntryPointToken value.
		  void setEntryPointToken(dword dwValue); // EXPORT
		  /// Change the COM+ descriptor's Resources (VirtualAddress) value.
		  void setResourcesVa(dword dwValue); // EXPORT
		  /// Change the COM+ descriptor's Resources (Size) value.
		  void setResourcesSize(dword dwValue); // EXPORT
		  /// Change the COM+ descriptor's StrongNameSignatureVa (VirtualAddress) value.
		  void setStrongNameSignatureVa(dword dwValue); // EXPORT
		  /// Change the COM+ descriptor's StrongNameSignatureVa (Size) value.
		  void setStrongNameSignagureSize(dword dwValue); // EXPORT
		  /// Change the COM+ descriptor's CodeManagerTable (VirtualAddress) value.
		  void setCodeManagerTableVa(dword dwValue); // EXPORT
		  /// Change the COM+ descriptor's CodeManagerTable (Size) value.
		  void setCodeManagerTableSize(dword dwValue); // EXPORT
		  /// Change the COM+ descriptor's VTableFixups (VirtualAddress) value.
		  void setVTableFixupsVa(dword dwValue); // EXPORT
		  /// Change the COM+ descriptor's VTableFixups (Size) value.
		  void setVTableFixupsSize(dword dwValue); // EXPORT
		  /// Change the COM+ descriptor's ExportAddressTableJumps (VirtualAddress) value.
		  void setExportAddressTableJumpsVa(dword dwValue); // EXPORT
		  /// Change the COM+ descriptor's ExportAddressTableJumps (Size) value.
		  void setExportAddressTableJumpsSize(dword dwValue); // EXPORT
		  /// Change the COM+ descriptor's ManagedNativeHeader (VirtualAddress) value.
		  void setManagedNativeHeaderVa(dword dwValue); // EXPORT
		  /// Change the COM+ descriptor's ManagedNativeHeader (Size) value.
		  void setManagedNativeHeaderSize(dword dwValue); // EXPORT
	};
}
#endif
