//===-- Materializer.h ------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef lldb_Materializer_h
#define lldb_Materializer_h

#include "lldb/lldb-private-types.h"
#include "lldb/Core/Error.h"
#include "lldb/Expression/IRMemoryMap.h"
#include "lldb/Host/Mutex.h"
#include "lldb/Symbol/SymbolContext.h"

#include <vector>

namespace lldb_private
{
    
class Materializer
{
public:
    Materializer ();
    
    class Dematerializer
    {
    public:
        void Dematerialize(Error &err);
    private:
        friend class Materializer;

        Dematerializer (Materializer &materializer,
                        lldb::StackFrameSP &frame_sp,
                        IRMemoryMap &map,
                        lldb::addr_t process_address) :
            m_materializer(materializer),
            m_frame_wp(frame_sp),
            m_map(map),
            m_process_address(process_address)
        {
        }
        
        Materializer       &m_materializer;
        lldb::StackFrameWP  m_frame_wp;
        IRMemoryMap        &m_map;
        lldb::addr_t        m_process_address;
    };
    
    Dematerializer Materialize (lldb::StackFrameSP &frame_sp, lldb::ClangExpressionVariableSP &result_sp, IRMemoryMap &map, lldb::addr_t process_address, Error &err);
    
    uint32_t AddPersistentVariable (lldb::ClangExpressionVariableSP &persistent_variable_sp, Error &err);
    uint32_t AddVariable (lldb::VariableSP &variable_sp, Error &err);
    uint32_t AddResultVariable (const ClangASTType &type, Error &err);
    uint32_t AddSymbol (const Symbol &symbol_sp, Error &err);
    uint32_t AddRegister (const RegisterInfo &register_info, Error &err);
    
    uint32_t GetStructAlignment ();

    class Entity
    {
    public:
        Entity () :
            m_alignment(1),
            m_size(0),
            m_offset(0)
        {
        }
        
        virtual ~Entity ()
        {
        }
        
        virtual void Materialize (lldb::StackFrameSP &frame_sp, IRMemoryMap &map, lldb::addr_t process_address, Error &err) = 0;
        virtual void Dematerialize (lldb::StackFrameSP &frame_sp, IRMemoryMap &map, lldb::addr_t process_address, Error &err) = 0;
        
        uint32_t GetAlignment ()
        {
            return m_alignment;
        }
        
        uint32_t GetSize ()
        {
            return m_size;
        }
        
        uint32_t GetOffset ()
        {
            return m_offset;
        }
        
        void SetOffset (uint32_t offset)
        {
            m_offset = offset;
        }
    protected:
        void SetSizeAndAlignmentFromType (ClangASTType &type);
        
        uint32_t    m_alignment;
        uint32_t    m_size;
        uint32_t    m_offset;
    };

private:
    uint32_t AddStructMember (Entity &entity);
    
    typedef std::unique_ptr<Entity> EntityUP;
    typedef std::vector<EntityUP>   EntityVector;
    
    unsigned            m_result_index;
    Mutex               m_needs_dematerialize;
    EntityVector        m_entities;
    uint32_t            m_current_offset;
    uint32_t            m_struct_alignment;
};
    
}

#endif
//===-- Materializer.h ------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef lldb_Materializer_h
#define lldb_Materializer_h

#include "lldb/lldb-private-types.h"
#include "lldb/Core/Error.h"
#include "lldb/Expression/IRMemoryMap.h"
#include "lldb/Host/Mutex.h"
#include "lldb/Symbol/SymbolContext.h"

#include <vector>

namespace lldb_private
{
    
class Materializer
{
public:
    Materializer ();
    
    class Dematerializer
    {
    public:
        void Dematerialize(Error &err);
    private:
        friend class Materializer;

        Dematerializer (Materializer &materializer,
                        lldb::StackFrameSP &frame_sp,
                        IRMemoryMap &map,
                        lldb::addr_t process_address) :
            m_materializer(materializer),
            m_frame_wp(frame_sp),
            m_map(map),
            m_process_address(process_address)
        {
        }
        
        Materializer       &m_materializer;
        lldb::StackFrameWP  m_frame_wp;
        IRMemoryMap        &m_map;
        lldb::addr_t        m_process_address;
    };
    
    Dematerializer Materialize (lldb::StackFrameSP &frame_sp, lldb::ClangExpressionVariableSP &result_sp, IRMemoryMap &map, lldb::addr_t process_address, Error &err);
    
    uint32_t AddPersistentVariable (lldb::ClangExpressionVariableSP &persistent_variable_sp, Error &err);
    uint32_t AddVariable (lldb::VariableSP &variable_sp, Error &err);
    uint32_t AddResultVariable (const ClangASTType &type, Error &err);
    uint32_t AddSymbol (const Symbol &symbol_sp, Error &err);
    uint32_t AddRegister (const RegisterInfo &register_info, Error &err);
    
    uint32_t GetStructAlignment ();

    class Entity
    {
    public:
        Entity () :
            m_alignment(1),
            m_size(0),
            m_offset(0)
        {
        }
        
        virtual ~Entity ()
        {
        }
        
        virtual void Materialize (lldb::StackFrameSP &frame_sp, IRMemoryMap &map, lldb::addr_t process_address, Error &err) = 0;
        virtual void Dematerialize (lldb::StackFrameSP &frame_sp, IRMemoryMap &map, lldb::addr_t process_address, Error &err) = 0;
        
        uint32_t GetAlignment ()
        {
            return m_alignment;
        }
        
        uint32_t GetSize ()
        {
            return m_size;
        }
        
        uint32_t GetOffset ()
        {
            return m_offset;
        }
        
        void SetOffset (uint32_t offset)
        {
            m_offset = offset;
        }
    protected:
        void SetSizeAndAlignmentFromType (ClangASTType &type);
        
        uint32_t    m_alignment;
        uint32_t    m_size;
        uint32_t    m_offset;
    };

private:
    uint32_t AddStructMember (Entity &entity);
    
    typedef std::unique_ptr<Entity> EntityUP;
    typedef std::vector<EntityUP>   EntityVector;
    
    unsigned            m_result_index;
    Mutex               m_needs_dematerialize;
    EntityVector        m_entities;
    uint32_t            m_current_offset;
    uint32_t            m_struct_alignment;
};
    
}

#endif
//===-- Materializer.h ------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef lldb_Materializer_h
#define lldb_Materializer_h

#include "lldb/lldb-private-types.h"
#include "lldb/Core/Error.h"
#include "lldb/Expression/IRMemoryMap.h"
#include "lldb/Host/Mutex.h"
#include "lldb/Symbol/SymbolContext.h"

#include <vector>

namespace lldb_private
{
    
class Materializer
{
public:
    Materializer ();
    
    class Dematerializer
    {
    public:
        void Dematerialize(Error &err);
    private:
        friend class Materializer;

        Dematerializer (Materializer &materializer,
                        lldb::StackFrameSP &frame_sp,
                        IRMemoryMap &map,
                        lldb::addr_t process_address) :
            m_materializer(materializer),
            m_frame_wp(frame_sp),
            m_map(map),
            m_process_address(process_address)
        {
        }
        
        Materializer       &m_materializer;
        lldb::StackFrameWP  m_frame_wp;
        IRMemoryMap        &m_map;
        lldb::addr_t        m_process_address;
    };
    
    Dematerializer Materialize (lldb::StackFrameSP &frame_sp, lldb::ClangExpressionVariableSP &result_sp, IRMemoryMap &map, lldb::addr_t process_address, Error &err);
    
    uint32_t AddPersistentVariable (lldb::ClangExpressionVariableSP &persistent_variable_sp, Error &err);
    uint32_t AddVariable (lldb::VariableSP &variable_sp, Error &err);
    uint32_t AddResultVariable (const ClangASTType &type, Error &err);
    uint32_t AddSymbol (const Symbol &symbol_sp, Error &err);
    uint32_t AddRegister (const RegisterInfo &register_info, Error &err);
    
    uint32_t GetStructAlignment ()
    {
        return m_struct_alignment;
    }
    
    uint32_t GetStructSize ()
    {
        return m_current_offset;
    }
    
    uint32_t GetNumEntities ()
    {
        return m_entities.size();
    }

    class Entity
    {
    public:
        Entity () :
            m_alignment(1),
            m_size(0),
            m_offset(0)
        {
        }
        
        virtual ~Entity ()
        {
        }
        
        virtual void Materialize (lldb::StackFrameSP &frame_sp, IRMemoryMap &map, lldb::addr_t process_address, Error &err) = 0;
        virtual void Dematerialize (lldb::StackFrameSP &frame_sp, IRMemoryMap &map, lldb::addr_t process_address, Error &err) = 0;
        
        uint32_t GetAlignment ()
        {
            return m_alignment;
        }
        
        uint32_t GetSize ()
        {
            return m_size;
        }
        
        uint32_t GetOffset ()
        {
            return m_offset;
        }
        
        void SetOffset (uint32_t offset)
        {
            m_offset = offset;
        }
    protected:
        void SetSizeAndAlignmentFromType (ClangASTType &type);
        
        uint32_t    m_alignment;
        uint32_t    m_size;
        uint32_t    m_offset;
    };

private:
    uint32_t AddStructMember (Entity &entity);
    
    typedef std::unique_ptr<Entity> EntityUP;
    typedef std::vector<EntityUP>   EntityVector;
    
    unsigned            m_result_index;
    Mutex               m_needs_dematerialize;
    EntityVector        m_entities;
    uint32_t            m_current_offset;
    uint32_t            m_struct_alignment;
};
    
}

#endif
