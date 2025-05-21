import React, { useState } from 'react';

interface TabsProps {
  children: React.ReactElement[];
  labels: string[];
}

const Tabs: React.FC<TabsProps> = ({ children, labels }) => {
  const [activeTab, setActiveTab] = useState(0);
  
  return (
    <div>
      <div className="border-b border-gray-200">
        <nav className="flex -mb-px overflow-x-auto">
          {labels.map((label, index) => (
            <button
              key={index}
              onClick={() => setActiveTab(index)}
              className={`py-4 px-6 font-medium text-sm border-b-2 whitespace-nowrap transition-colors ${
                activeTab === index
                  ? 'border-blue-600 text-blue-600'
                  : 'border-transparent text-gray-500 hover:text-gray-700 hover:border-gray-300'
              }`}
            >
              {label}
            </button>
          ))}
        </nav>
      </div>
      
      <div>
        {React.Children.map(children, (child, index) => (
          <div className={activeTab === index ? 'block' : 'hidden'}>
            {child}
          </div>
        ))}
      </div>
    </div>
  );
};

export default Tabs;