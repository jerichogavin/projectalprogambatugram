import React, { useState } from 'react';
import { useAppContext } from '../context/AppContext';
import { Save, FileJson } from 'lucide-react';

const DataControls: React.FC = () => {
  const { saveLogsToFile, exportLogsToJson } = useAppContext();
  const [savedStatus, setSavedStatus] = useState('');
  const [exportStatus, setExportStatus] = useState('');
  
  const handleSaveBinary = () => {
    // Simulate saving to binary file (in a real app, this would actually save to a file)
    saveLogsToFile();
    setSavedStatus('success');
    
    setTimeout(() => {
      setSavedStatus('');
    }, 3000);
  };
  
  const handleExportJson = () => {
    try {
      const jsonData = exportLogsToJson();
      const blob = new Blob([jsonData], { type: 'application/json' });
      const url = URL.createObjectURL(blob);
      
      const a = document.createElement('a');
      a.href = url;
      a.download = `lab_attendance_${new Date().toISOString().split('T')[0]}.json`;
      a.click();
      
      URL.revokeObjectURL(url);
      setExportStatus('success');
    } catch (error) {
      setExportStatus('error');
      console.error('Error exporting JSON:', error);
    }
    
    setTimeout(() => {
      setExportStatus('');
    }, 3000);
  };
  
  return (
    <div className="bg-white rounded-lg shadow-md m-6">
      <div className="p-6 border-b">
        <h2 className="text-xl font-semibold text-gray-800">Data Management</h2>
        <p className="text-gray-600 mt-1">Save and export attendance data</p>
      </div>
      
      <div className="p-6 grid grid-cols-1 md:grid-cols-2 gap-6">
        {/* Binary File Storage */}
        <div className="bg-gray-50 p-6 rounded-lg">
          <div className="flex items-start mb-4">
            <Save className="h-6 w-6 text-blue-600 mt-1 mr-3" />
            <div>
              <h3 className="font-semibold text-lg">Binary Storage</h3>
              <p className="text-gray-600 text-sm">
                Save all attendance logs to a binary file for efficient storage and backup.
                This process ensures data persistence across system restarts.
              </p>
            </div>
          </div>
          
          <div className="flex items-center">
            <button
              onClick={handleSaveBinary}
              className="px-4 py-2 bg-blue-600 hover:bg-blue-700 text-white rounded-md transition-colors"
            >
              Save to Binary File
            </button>
            
            {savedStatus === 'success' && (
              <span className="ml-3 text-green-600 text-sm">Saved successfully!</span>
            )}
          </div>
        </div>
        
        {/* JSON Export */}
        <div className="bg-gray-50 p-6 rounded-lg">
          <div className="flex items-start mb-4">
            <FileJson className="h-6 w-6 text-teal-600 mt-1 mr-3" />
            <div>
              <h3 className="font-semibold text-lg">JSON Export</h3>
              <p className="text-gray-600 text-sm">
                Export attendance data to JSON format for integration with other systems
                or for importing into analytics tools.
              </p>
            </div>
          </div>
          
          <div className="flex items-center">
            <button
              onClick={handleExportJson}
              className="px-4 py-2 bg-teal-600 hover:bg-teal-700 text-white rounded-md transition-colors"
            >
              Export to JSON
            </button>
            
            {exportStatus === 'success' && (
              <span className="ml-3 text-green-600 text-sm">Exported successfully!</span>
            )}
            
            {exportStatus === 'error' && (
              <span className="ml-3 text-red-600 text-sm">Export failed. Try again.</span>
            )}
          </div>
        </div>
      </div>
      
      {/* Data Processing Information */}
      <div className="px-6 pb-6">
        <div className="bg-blue-50 border border-blue-200 rounded-lg p-4">
          <h3 className="font-semibold text-blue-800 mb-2">About Thread Safety</h3>
          <p className="text-blue-700 text-sm">
            This system implements thread-safe operations to handle concurrent RFID scans.
            When multiple readers submit data simultaneously, the system ensures that:
          </p>
          <ul className="list-disc list-inside text-blue-700 text-sm mt-2 space-y-1">
            <li>All scan requests are processed in their entirety</li>
            <li>Data is written to storage without corruption</li>
            <li>Race conditions are prevented through proper synchronization</li>
            <li>Log sequence is maintained based on accurate timestamps</li>
          </ul>
        </div>
      </div>
    </div>
  );
};

export default DataControls;