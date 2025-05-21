import React, { createContext, useContext, useState, useEffect, ReactNode } from 'react';
import { User, Lab, RFIDReader, ScanLog, ServerStats } from '../types';
import { mockUsers, mockLabs, mockReaders, mockLogs } from '../data/mockData';

interface AppContextType {
  users: User[];
  labs: Lab[];
  readers: RFIDReader[];
  logs: ScanLog[];
  serverStats: ServerStats;
  addScanLog: (userId: string, readerId: string, direction: 'in' | 'out') => void;
  searchLogs: (query: string, filterBy: 'userId' | 'rfidId' | 'labId') => ScanLog[];
  sortLogs: (sortBy: 'timestamp' | 'userName' | 'labName') => void;
  saveLogsToFile: () => void;
  exportLogsToJson: () => string;
  simulateRFIDScan: (rfidId: string, readerId: string) => void;
}

const AppContext = createContext<AppContextType | undefined>(undefined);

export const useAppContext = () => {
  const context = useContext(AppContext);
  if (context === undefined) {
    throw new Error('useAppContext must be used within an AppProvider');
  }
  return context;
};

interface AppProviderProps {
  children: ReactNode;
}

export const AppProvider: React.FC<AppProviderProps> = ({ children }) => {
  const [users] = useState<User[]>(mockUsers);
  const [labs] = useState<Lab[]>(mockLabs);
  const [readers, setReaders] = useState<RFIDReader[]>(mockReaders);
  const [logs, setLogs] = useState<ScanLog[]>(mockLogs);
  const [sortOrder, setSortOrder] = useState<'asc' | 'desc'>('desc');
  
  // Server statistics
  const [serverStats, setServerStats] = useState<ServerStats>({
    totalScansToday: 0,
    activeUsers: 0,
    totalUsers: users.length,
    lastScanTime: null,
  });

  // Update server stats whenever logs change
  useEffect(() => {
    const today = new Date();
    today.setHours(0, 0, 0, 0);
    
    const scansToday = logs.filter(log => log.timestamp >= today).length;
    
    // Find unique users who are currently in labs (have an 'in' scan without a matching 'out')
    const activeUserIds = new Set<string>();
    
    logs.forEach(log => {
      if (log.direction === 'in') {
        activeUserIds.add(log.userId);
      } else if (log.direction === 'out') {
        activeUserIds.delete(log.userId);
      }
    });
    
    const lastScan = logs.length > 0 ? logs[logs.length - 1].timestamp : null;
    
    setServerStats({
      totalScansToday: scansToday,
      activeUsers: activeUserIds.size,
      totalUsers: users.length,
      lastScanTime: lastScan,
    });
  }, [logs, users.length]);

  // Update reader status periodically
  useEffect(() => {
    const updateReaderStatus = () => {
      setReaders(prevReaders => {
        return prevReaders.map(reader => {
          // Randomly change status sometimes
          if (Math.random() > 0.95) {
            const statuses: RFIDReader['status'][] = ['online', 'offline', 'error'];
            const newStatus = statuses[Math.floor(Math.random() * statuses.length)];
            return { ...reader, status: newStatus, lastPing: new Date() };
          }
          return reader;
        });
      });
    };
    
    const interval = setInterval(updateReaderStatus, 10000);
    return () => clearInterval(interval);
  }, []);

  // Add a new scan log
  const addScanLog = (userId: string, readerId: string, direction: 'in' | 'out') => {
    const user = users.find(u => u.id === userId);
    const reader = readers.find(r => r.id === readerId);
    
    if (!user || !reader) return;
    
    const lab = labs.find(l => l.id === reader.labId);
    if (!lab) return;
    
    const newLog: ScanLog = {
      id: `log-${logs.length + 1}`,
      userId: user.id,
      userName: user.name,
      userRole: user.role,
      rfidId: user.rfidId,
      readerId: reader.id,
      labId: lab.id,
      labName: lab.name,
      timestamp: new Date(),
      direction,
    };
    
    // Simulate concurrent access with setTimeout
    setTimeout(() => {
      setLogs(prevLogs => [...prevLogs, newLog]);
    }, Math.random() * 100);
  };

  // Search logs by various criteria
  const searchLogs = (query: string, filterBy: 'userId' | 'rfidId' | 'labId') => {
    if (!query) return logs;
    
    return logs.filter(log => {
      if (filterBy === 'userId') return log.userName.toLowerCase().includes(query.toLowerCase());
      if (filterBy === 'rfidId') return log.rfidId.toLowerCase().includes(query.toLowerCase());
      if (filterBy === 'labId') return log.labName.toLowerCase().includes(query.toLowerCase());
      return false;
    });
  };

  // Sort logs by different criteria
  const sortLogs = (sortBy: 'timestamp' | 'userName' | 'labName') => {
    const newOrder = sortOrder === 'asc' ? 'desc' : 'asc';
    setSortOrder(newOrder);
    
    setLogs(prevLogs => {
      const sortedLogs = [...prevLogs];
      
      if (sortBy === 'timestamp') {
        sortedLogs.sort((a, b) => {
          return newOrder === 'asc' 
            ? a.timestamp.getTime() - b.timestamp.getTime()
            : b.timestamp.getTime() - a.timestamp.getTime();
        });
      } else if (sortBy === 'userName') {
        sortedLogs.sort((a, b) => {
          return newOrder === 'asc'
            ? a.userName.localeCompare(b.userName)
            : b.userName.localeCompare(a.userName);
        });
      } else if (sortBy === 'labName') {
        sortedLogs.sort((a, b) => {
          return newOrder === 'asc'
            ? a.labName.localeCompare(b.labName)
            : b.labName.localeCompare(a.labName);
        });
      }
      
      return sortedLogs;
    });
  };

  // Save logs to binary file (simulated)
  const saveLogsToFile = () => {
    console.log('Saving logs to binary file...');
    // In a real system, this would save to an actual binary file
    // For the demo, we'll just log to console
  };

  // Export logs to JSON
  const exportLogsToJson = () => {
    const jsonData = JSON.stringify(logs, null, 2);
    return jsonData;
  };

  // Simulate an RFID scan
  const simulateRFIDScan = (rfidId: string, readerId: string) => {
    const user = users.find(u => u.rfidId === rfidId);
    if (!user) return;
    
    // Determine if the user is entering or leaving
    // Check if their last scan was "in" or "out"
    const userLogs = logs
      .filter(log => log.userId === user.id)
      .sort((a, b) => b.timestamp.getTime() - a.timestamp.getTime());
    
    const lastDirection = userLogs.length > 0 ? userLogs[0].direction : 'out';
    const newDirection = lastDirection === 'in' ? 'out' : 'in';
    
    addScanLog(user.id, readerId, newDirection);
  };

  const value = {
    users,
    labs,
    readers,
    logs,
    serverStats,
    addScanLog,
    searchLogs,
    sortLogs,
    saveLogsToFile,
    exportLogsToJson,
    simulateRFIDScan,
  };

  return <AppContext.Provider value={value}>{children}</AppContext.Provider>;
};