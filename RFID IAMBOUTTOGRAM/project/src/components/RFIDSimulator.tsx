import React, { useState } from 'react';
import { useAppContext } from '../context/AppContext';
import { Zap, AlertTriangle } from 'lucide-react';

const RFIDSimulator: React.FC = () => {
  const { users, readers, simulateRFIDScan } = useAppContext();
  
  const [selectedUser, setSelectedUser] = useState('');
  const [selectedReader, setSelectedReader] = useState('');
  const [scanStatus, setScanStatus] = useState<'idle' | 'success' | 'error'>('idle');
  const [message, setMessage] = useState('');
  
  // Get only online readers
  const onlineReaders = readers.filter(reader => reader.status === 'online');
  
  const handleScan = () => {
    if (!selectedUser || !selectedReader) {
      setScanStatus('error');
      setMessage('Please select both a user and a reader');
      return;
    }
    
    const user = users.find(u => u.id === selectedUser);
    if (!user) {
      setScanStatus('error');
      setMessage('Selected user not found');
      return;
    }
    
    // Simulate multiple concurrent scans
    setTimeout(() => {
      simulateRFIDScan(user.rfidId, selectedReader);
      setScanStatus('success');
      setMessage(`RFID scan successful for ${user.name}`);
      
      // Reset after 3 seconds
      setTimeout(() => {
        setScanStatus('idle');
        setMessage('');
      }, 3000);
    }, Math.random() * 300);
  };
  
  return (
    <div className="bg-white rounded-lg shadow-md m-6">
      <div className="p-6 border-b">
        <h2 className="text-xl font-semibold text-gray-800">RFID Scanner Simulator</h2>
        <p className="text-gray-600 mt-1">Test the system by simulating RFID scans</p>
      </div>
      
      <div className="p-6">
        <div className="grid grid-cols-1 md:grid-cols-2 gap-6">
          {/* Card Reader Section */}
          <div className="space-y-4">
            <h3 className="font-medium text-lg">Select Card Reader</h3>
            
            {onlineReaders.length > 0 ? (
              <div className="grid grid-cols-1 sm:grid-cols-2 gap-3">
                {onlineReaders.map(reader => (
                  <div
                    key={reader.id}
                    onClick={() => setSelectedReader(reader.id)}
                    className={`border rounded-lg p-4 cursor-pointer transition-colors ${
                      selectedReader === reader.id 
                        ? 'border-teal-500 bg-teal-50' 
                        : 'border-gray-200 hover:border-gray-300'
                    }`}
                  >
                    <div className="flex items-center">
                      <div className={`w-3 h-3 rounded-full mr-2 ${
                        reader.status === 'online' ? 'bg-green-500' : 'bg-gray-400'
                      }`}></div>
                      <span className="font-medium">Reader {reader.id}</span>
                    </div>
                  </div>
                ))}
              </div>
            ) : (
              <div className="flex items-center p-4 bg-yellow-50 text-yellow-800 rounded-md">
                <AlertTriangle className="h-5 w-5 mr-2" />
                <span>No online readers available</span>
              </div>
            )}
          </div>
          
          {/* User Selection Section */}
          <div className="space-y-4">
            <h3 className="font-medium text-lg">Select User</h3>
            <select
              value={selectedUser}
              onChange={(e) => setSelectedUser(e.target.value)}
              className="w-full border rounded-md px-3 py-2"
            >
              <option value="">Select a user</option>
              {users.map(user => (
                <option key={user.id} value={user.id}>
                  {user.name} ({user.role}) - RFID: {user.rfidId}
                </option>
              ))}
            </select>
          </div>
        </div>
        
        {/* Scan Button and Status */}
        <div className="mt-8 flex flex-col items-center">
          <button
            onClick={handleScan}
            disabled={scanStatus !== 'idle'}
            className={`flex items-center px-6 py-3 rounded-md text-white font-medium transition-colors ${
              scanStatus === 'idle' 
                ? 'bg-gradient-to-r from-blue-600 to-indigo-700 hover:from-blue-700 hover:to-indigo-800' 
                : 'bg-gray-400 cursor-not-allowed'
            }`}
          >
            <Zap className="h-5 w-5 mr-2" />
            Simulate RFID Scan
          </button>
          
          {message && (
            <div className={`mt-4 px-4 py-2 rounded-md ${
              scanStatus === 'success' ? 'bg-green-100 text-green-800' : 'bg-red-100 text-red-800'
            }`}>
              {message}
            </div>
          )}
        </div>
        
        {/* Visual RFID Card */}
        {selectedUser && (
          <div className="mt-8">
            <div className="max-w-sm mx-auto">
              <div className="bg-gradient-to-r from-blue-500 to-indigo-600 rounded-xl p-1">
                <div className="bg-white rounded-lg p-4">
                  <div className="flex justify-between items-start">
                    <div>
                      <h4 className="font-bold text-lg">
                        {users.find(u => u.id === selectedUser)?.name}
                      </h4>
                      <p className="text-gray-500 text-sm capitalize">
                        {users.find(u => u.id === selectedUser)?.role}
                      </p>
                    </div>
                    <div className="bg-gray-100 rounded-md px-2 py-1 text-xs font-mono">
                      {users.find(u => u.id === selectedUser)?.rfidId}
                    </div>
                  </div>
                  
                  <div className="mt-6 flex items-center justify-center">
                    <div className="w-20 h-16 bg-gray-200 rounded-md flex items-center justify-center">
                      <svg className="w-10 h-10 text-gray-400" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2">
                        <path d="M9 3H5a2 2 0 00-2 2v4m6-6h10a2 2 0 012 2v4M9 3v18m0 0h10a2 2 0 002-2V9M9 21H5a2 2 0 01-2-2V9m0 0h18" />
                      </svg>
                    </div>
                  </div>
                  
                  <div className="mt-4 grid grid-cols-4 gap-1">
                    {Array(16).fill(0).map((_, i) => (
                      <div key={i} className="h-1 bg-gray-300 rounded"></div>
                    ))}
                  </div>
                </div>
              </div>
            </div>
          </div>
        )}
      </div>
    </div>
  );
};

export default RFIDSimulator;