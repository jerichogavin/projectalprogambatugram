import React from 'react';
import { useAppContext } from '../context/AppContext';
import { Server, Users, Clock } from 'lucide-react';

const Header: React.FC = () => {
  const { serverStats } = useAppContext();
  
  return (
    <header className="bg-gradient-to-r from-indigo-900 to-blue-800 text-white shadow-md">
      <div className="container mx-auto px-4 py-4">
        <div className="flex flex-col md:flex-row md:items-center md:justify-between">
          <div className="flex items-center mb-4 md:mb-0">
            <Server className="h-8 w-8 mr-3" />
            <h1 className="text-2xl font-bold">RFID Lab Attendance System</h1>
          </div>
          
          <div className="flex flex-wrap gap-4">
            <div className="flex items-center bg-blue-800/50 px-3 py-2 rounded-lg">
              <Users className="h-5 w-5 mr-2" />
              <div>
                <div className="text-xs text-blue-200">Active Users</div>
                <div className="font-semibold">{serverStats.activeUsers} / {serverStats.totalUsers}</div>
              </div>
            </div>
            
            <div className="flex items-center bg-blue-800/50 px-3 py-2 rounded-lg">
              <svg className="h-5 w-5 mr-2" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2">
                <path d="M10 6H5a2 2 0 00-2 2v9a2 2 0 002 2h14a2 2 0 002-2V8a2 2 0 00-2-2h-5m-4 0V5a2 2 0 114 0v1m-4 0a2 2 0 104 0m-5 8a2 2 0 100-4 2 2 0 000 4zm0 0c1.306 0 2.417.835 2.83 2M9 14a3.001 3.001 0 00-2.83 2M15 11h3m-3 4h2" />
              </svg>
              <div>
                <div className="text-xs text-blue-200">Scans Today</div>
                <div className="font-semibold">{serverStats.totalScansToday}</div>
              </div>
            </div>
            
            <div className="flex items-center bg-blue-800/50 px-3 py-2 rounded-lg">
              <Clock className="h-5 w-5 mr-2" />
              <div>
                <div className="text-xs text-blue-200">Last Scan</div>
                <div className="font-semibold">
                  {serverStats.lastScanTime 
                    ? new Date(serverStats.lastScanTime).toLocaleTimeString() 
                    : 'None'}
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    </header>
  );
};

export default Header;