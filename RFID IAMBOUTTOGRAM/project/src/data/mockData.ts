import { User, Lab, RFIDReader, ScanLog } from '../types';

// Mock Users
export const mockUsers: User[] = [
  { id: '1', name: 'John Doe', role: 'student', rfidId: 'RF001' },
  { id: '2', name: 'Jane Smith', role: 'student', rfidId: 'RF002' },
  { id: '3', name: 'Dr. Robert Brown', role: 'staff', rfidId: 'RF003' },
  { id: '4', name: 'Emma Wilson', role: 'student', rfidId: 'RF004' },
  { id: '5', name: 'Prof. Sarah Johnson', role: 'staff', rfidId: 'RF005' },
  { id: '6', name: 'Michael Chen', role: 'student', rfidId: 'RF006' },
  { id: '7', name: 'Admin User', role: 'admin', rfidId: 'RF007' },
];

// Mock Labs
export const mockLabs: Lab[] = [
  { id: '1', name: 'Computer Science Lab', location: 'Building A, Floor 2', capacity: 30 },
  { id: '2', name: 'Biomedical Lab', location: 'Building B, Floor 1', capacity: 25 },
  { id: '3', name: 'Electronics Lab', location: 'Building A, Floor 3', capacity: 20 },
  { id: '4', name: 'Research Lab', location: 'Building C, Floor 2', capacity: 15 },
];

// Mock RFID Readers
export const mockReaders: RFIDReader[] = [
  { id: '1', labId: '1', status: 'online', lastPing: new Date() },
  { id: '2', labId: '1', status: 'online', lastPing: new Date() },
  { id: '3', labId: '2', status: 'online', lastPing: new Date() },
  { id: '4', labId: '3', status: 'offline', lastPing: new Date(Date.now() - 86400000) },
  { id: '5', labId: '4', status: 'error', lastPing: new Date() },
];

// Generate some initial scan logs
const generateInitialLogs = (): ScanLog[] => {
  const logs: ScanLog[] = [];
  
  // Create logs for the past week
  for (let i = 0; i < 50; i++) {
    const randomUser = mockUsers[Math.floor(Math.random() * mockUsers.length)];
    const randomLab = mockLabs[Math.floor(Math.random() * mockLabs.length)];
    const randomReader = mockReaders.find(reader => reader.labId === randomLab.id) || mockReaders[0];
    
    // Random time in the past week
    const randomTime = new Date(Date.now() - Math.floor(Math.random() * 7 * 24 * 60 * 60 * 1000));
    
    // Entry log
    logs.push({
      id: `log-${logs.length + 1}`,
      userId: randomUser.id,
      userName: randomUser.name,
      userRole: randomUser.role,
      rfidId: randomUser.rfidId,
      readerId: randomReader.id,
      labId: randomLab.id,
      labName: randomLab.name,
      timestamp: randomTime,
      direction: 'in'
    });
    
    // Exit log (1-3 hours later)
    const exitTime = new Date(randomTime.getTime() + (1 + Math.random() * 2) * 60 * 60 * 1000);
    logs.push({
      id: `log-${logs.length + 1}`,
      userId: randomUser.id,
      userName: randomUser.name,
      userRole: randomUser.role,
      rfidId: randomUser.rfidId,
      readerId: randomReader.id,
      labId: randomLab.id,
      labName: randomLab.name,
      timestamp: exitTime,
      direction: 'out'
    });
  }
  
  // Sort by timestamp
  return logs.sort((a, b) => a.timestamp.getTime() - b.timestamp.getTime());
};

export const mockLogs: ScanLog[] = generateInitialLogs();