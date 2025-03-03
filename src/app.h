
class VulkanApp {
    public: 
        static VulkanApp* getInstance();
        void run();
        ~VulkanApp();
            
    private:
        VulkanApp() = default; 

        inline static VulkanApp*    _m_app_instance = nullptr;
};

