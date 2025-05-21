import React from 'react';
import { useAppContext } from '../context/AppContext';
import { AlertTriangle, AlertCircle, CheckCircle } from 'lucide-react';

const Dashboard: React.FC = () => {
  const { readers, labs, logs } = useAppContext();
  
  // Calculate percentage of active readers
  const activeReaders = readers.filter(r => r.status === 'online').length;
  const totalReaders = readers.length;
  const activePercentage = Math.round((activeReaders / totalReaders) * 100);
  
  // Get latest logs (last 5)
  const latestLogs = [...logs]
    .sort((a, b) => b.timestamp.getTime() - a.timestamp.getTime())
    .slice(0, 5);
  
  // Labs with their current occupancy
  const labOccupancy = labs.map(lab => {
    // Find users currently in this lab
    const usersInLab = new Set<string>();
    
    // Process all logs chronologically
    [...logs].sort((a, b) => a.timestamp.getTime() - b.timestamp.getTime())
      .forEach(log => {
        if (log.labId === lab.id) {
          if (log.direction === 'in') {
            usersInLab.add(log.userId);
          } else if (log.direction === 'out') {
            usersInLab.delete(log.userId);
          }
        }
      });
    
    return {
      ...lab,
      currentOccupancy: usersInLab.size,
      occupancyPercentage: Math.round((usersInLab.size / lab.capacity) * 100)
    };
  });
  
  return (
    <div className="grid grid-cols-1 lg:grid-cols-3 gap-6 p-6">
      {/* System Status Card */}
      <div className="bg-white rounded-lg shadow-md p-6">
        <h2 className="text-xl font-semibold mb-4 text-gray-800">System Status</h2>
        
        <div className="flex items-center mb-4">
          <div className="w-full bg-gray-200 rounded-full h-4">
            <div 
              className={`h-4 rounded-full ${
                activePercentage > 80 ? 'bg-green-500' : 
                activePercentage > 50 ? 'bg-yellow-500' : 'bg-red-500'
              }`}
              style={{ width: `${activePercentage}%` }}
            ></div>
          </div>
          <span className="ml-2 font-medium text-gray-700">{activePercentage}%</span>
        </div>
        
        <div className="space-y-3">
          {readers.map(reader => (
            <div key={reader.id} className="flex items-center justify-between bg-gray-50 p-3 rounded-md">
              <div className="flex items-center">
                {reader.status === 'online' && <CheckCircle className="h-5 w-5 text-green-500 mr-2" />}
                {reader.status === 'offline' && <AlertCircle className="h-5 w-5 text-gray-500 mr-2" />}
                {reader.status === 'error' && <AlertTriangle className="h-5 w-5 text-red-500 mr-2" />}
                <span className="font-medium">Reader {reader.id}</span>
              </div>
              <span className={`text-sm px-2 py-1 rounded-full ${
                reader.status === 'online' ? 'bg-green-100 text-green-800' :
                reader.status === 'offline' ? 'bg-gray-100 text-gray-800' :
                'bg-red-100 text-red-800'
              }`}>
                {reader.status}
              </span>
            </div>
          ))}
        </div>
      </div>
      
      {/* Lab Occupancy Card */}
      <div className="bg-white rounded-lg shadow-md p-6">
        <h2 className="text-xl font-semibold mb-4 text-gray-800">Lab Occupancy</h2>
        
        <div className="space-y-4">
          {labOccupancy.map(lab => (
            <div key={lab.id} className="space-y-2">
              <div className="flex justify-between items-center">
                <span className="font-medium">{lab.name}</span>
                <span className="text-sm">{lab.currentOccupancy} / {lab.capacity}</span>
              </div>
              <div className="w-full bg-gray-200 rounded-full h-3">
                <div 
                  className={`h-3 rounded-full ${
                    lab.occupancyPercentage > 80 ? 'bg-red-500' : 
                    lab.occupancyPercentage > 50 ? 'bg-yellow-500' : 'bg-green-500'
                  }`}
                  style={{ width: `${lab.occupancyPercentage}%` }}
                ></div>
              </div>
              <p className="text-xs text-gray-500">{lab.location}</p>
            </div>
          ))}
        </div>
      </div>
      
      {/* Recent Activity Card */}
      <div className="bg-white rounded-lg shadow-md p-6">
        <h2 className="text-xl font-semibold mb-4 text-gray-800">Recent Activity</h2>
        
        <div className="space-y-3">
          {latestLogs.length > 0 ? (
            latestLogs.map(log => (
              <div key={log.id} className="border-l-4 pl-3 py-2 border-teal-500">
                <div className="flex justify-between">
                  <span className="font-medium">{log.userName}</span>
                  <span className={`text-xs px-2 py-1 rounded-full ${
                    log.direction === 'in' ? 'bg-green-100 text-green-800' : 'bg-blue-100 text-blue-800'
                  }`}>
                    {log.direction === 'in' ? 'Entered' : 'Exited'}
                  </span>
                </div>
                <div className="text-sm text-gray-500">
                  {log.labName} • {new Date(log.timestamp).toLocaleTimeString()}
                </div>
              </div>
            ))
          ) : (
            <p className="text-gray-500">No recent activity</p>
          )}
        </div>
      </div>
    </div>
  );
};

export default Dashboard;