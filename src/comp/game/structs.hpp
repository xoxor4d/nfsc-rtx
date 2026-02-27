#pragma once

// (c) structs from: https://github.com/MaxHwoy/hyperlinked

namespace comp::game
{
	enum class visible_state : std::uint8_t
	{
		outside,
		partial,
		inside,
	};

	struct options
	{
		int version; //0x0000 
		int size; //0x0004 
		int car_env_map_enabled; //0x0008 
		int performance_level_related1; //0x000C 
		int unused1; //0x0010 
		int unused2; //0x0014 
		int unused3; //0x0018 
		int bone_anims_enabled; //0x001C 
		int road_reflections_enabled; //0x0020 
		int unused4; //0x0024 
		int motion_blur_enabled; //0x0028 
		int flare_related_hardcoded; //0x002C 
		int texture_anims_enabled; //0x0030 
		int draw_particles; //0x0034 
		int world_lod_level; //0x0038 
		int world_lod_related; //0x003C 
		int car_lod_level; //0x0040 
		int fsaa_level; //0x0044 
		int rain_enabled; //0x0048 
		int texture_filtering; //0x004C 
		int racing_resolution; //0x0050 
		int first_time_start; //0x0054 
		int vsync; //0x0058 
		int visual_treatment; //0x005C 
		int shadow_detail; //0x0060 ---- actual last entry of options
		char pad_0x0064[0xC]; //0x0064
		int car_env_map_related; //0x0070 
		int performance_level_related2; //0x0074 
		char pad_0x0078[0xC]; //0x0078
		int bone_anims_supported; //0x0084 
		int combine_reflection_color_headlight_alpha; //0x0088 
		char pad_0x008C[0x4]; //0x008C
		int motion_blur_supported; //0x0090 
		char pad_0x0094[0x4]; //0x0094
		int texture_anims_supported; //0x0098 
		int particles_supported; //0x009C 
		int worldlodlevel_supported; //0x00A0 
		char pad_0x00A4[0x4]; //0x00A4
		int carlodlevel_supported; //0x00A8 
		int fsaa_supported; //0x00AC 
		int rain_supported; //0x00B0 
		int texturefiltering_supported; //0x00B4 
		char pad_0x00B8[0xC]; //0x00B8
		int visual_treatment_supported; //0x00C4 
		int shadow_detail_supported; //0x00C8 
		char pad_0x00CC[0x4]; //0x00CC
	}; //Size=0x00D0
	STATIC_ASSERT_OFFSET(options, rain_supported, 0xB0);

	struct vis_struct
	{
		int unk1;
		int some_flag;
		int unk3;
		int unk4;
		int unk5;
		int unk6;
		int unk7;
		int unk8;
		Vector4D origin;
		Vector4D plane_normal;
		float max_strength;
		std::int32_t preculler_section_number;
		int unk9;
		int unk10;
	}; STATIC_ASSERT_OFFSET(vis_struct, preculler_section_number, 0x44);

	struct tree_node
	{
		Vector bbox_min;
		std::uint16_t child_count;
		std::uint16_t pad;
		Vector bbox_max;
		std::int16_t children[10];
	};


	struct ViewTransform
	{
	    D3DXMATRIX ViewMatrix;
	    D3DXMATRIX ProjectionMatrix;
	    D3DXMATRIX ProjectionZBiasMatrix;
	    D3DXMATRIX ViewProjectionMatrix;
	    D3DXMATRIX ViewProjectionZBiasMatrix;
	};

	struct eViewPlatInterface
	{      
	    ViewTransform* m_pTransform;
	};


	struct material_data
	{
		float diffuse_power;
		float diffuse_clamp;
		float diffuse_flakes;
		float diffuse_vinyl_scale;
		float diffuse_min_scale;
		Vector diffuse_min;
		float diffuse_max_scale;
		Vector diffuse_max;
		float diffuse_min_a;
		float diffuse_max_a;
		float specular_power;
		float specular_flakes;
		float specular_vinyl_scale;
		float specular_min_scale;
		Vector specular_min;
		float specular_max_scale;
		Vector specular_max;
		float envmap_power;
		float envmap_clamp;
		float envmap_vinyl_scale;
		float envmap_min_scale;
		Vector envmap_min;
		float envmap_max_scale;
		Vector envmap_max;
		float vinyl_luminance_min_scale;
		float vinyl_luminance_max_scale;
	};

	struct material_instance
	{
		void* platform_info;
		int pad1;
		int pad2;
		std::uint32_t key;
		std::uint32_t version;
		char name[64];
		material_data material;
	};

	struct effect
	{
		DWORD dword0;
		int unk1;
		int unk2;
		int unk3;
		int unk4;
		DWORD dword14;
		BYTE gap18[44];
		ID3DXBaseEffect* fx;
		IDirect3DVertexDeclaration9* pidirect3dvertexdeclaration948;
		BYTE gap4C[5896];
		DWORD dword1754;
		DWORD dword1758;
		BYTE gap175C[24];
		DWORD dword1774;
		int unk5;
		int unk6;
		material_instance* last_used_light_material_;
		int unk8;
	};
	STATIC_ASSERT_OFFSET(effect, last_used_light_material_, 0x1780);

	struct camera
	{
		D3DXMATRIX view_matrix;
		Vector4D position;
		Vector4D direction;
		Vector4D target;
	};

	struct rain
	{
		char pad0[564];
		int no_rain; // 0x234
		int no_rain_ahead;
		int in_tunnel;
		int in_overpass;
		char pad1[72];
		float rain_intensity;
		float cloud_intensity;// 0x28C
		char pad2[13312];
		float desired_intensity;
		float desired_cloudyness;
		float desired_road_dampness;
		float road_dampness; // 0x36A0
	};
	STATIC_ASSERT_OFFSET(rain, rain_intensity, 0x28C);
	STATIC_ASSERT_OFFSET(rain, road_dampness, 0x36A0);

	struct view_base
	{
		void* platform_info; //0x0000 
		char* name; //0x0004 
		int id; //0x0008 
		BYTE active; //0x000C 
		char pad0[3]; //0x0010
		char pad1[48];
		camera* camera; //0x0040 
		char pad2[40];
		rain* rain; //0x006C 
		char pad_0x0070[0x10]; //0x0070
	};
	STATIC_ASSERT_OFFSET(view_base, rain, 0x6C);
	STATIC_ASSERT_SIZE(view_base, 0x80);

	template <typename T> class span final
	{
	public:
		inline span() = default;

		inline span(const span& other) = default;

		inline span(span&& other) = default;

		inline span& operator=(const span& other) = default;

		inline span& operator=(span&& other) = default;

		inline span(T* ptr, size_t length) : ptr_(ptr), length_(length)
		{
		}

		inline span(intptr_t address, size_t length) : ptr_(reinterpret_cast<T*>(address)), length_(length)
		{
		}

		inline span(uintptr_t address, size_t length) : ptr_(reinterpret_cast<T*>(address)), length_(length)
		{
		}

		template <size_t Length> inline span(T(&array)[Length]) : ptr_(reinterpret_cast<T*>(array)), length_(Length)
		{
		}

		template <typename Index> inline auto operator[](Index index) -> T&
		{
			assert(static_cast<size_t>(index) < this->length_);

			return this->ptr_[static_cast<size_t>(index)];
		}

		template <typename Index> inline auto operator[](Index index) const -> const T&
		{
			assert(static_cast<size_t>(index) < this->length_);

			return this->ptr_[static_cast<size_t>(index)];
		}

		inline auto length() const -> size_t
		{
			return this->length_;
		}

		inline bool is_empty() const
		{
			return this->length_ == 0u;
		}

		inline void clear()
		{
			::memset(this->ptr_, 0, this->length_ * sizeof(T));
		}

		inline void fill(const T& value)
		{
			std::fill_n(this->ptr_, this->length_, value);
		}

		inline auto begin() -> T*
		{
			return this->ptr_;
		}

		inline auto begin() const -> const T*
		{
			return this->ptr_;
		}

		inline auto end() -> T*
		{
			return this->ptr_ + this->length_;
		}

		inline auto end() const -> const T*
		{
			return this->ptr_ + this->length_;
		}

		inline auto slice(size_t start) -> span<T>
		{
			assert(start <= this->length_);

			return { this->ptr_ + start, this->length_ - start };
		}

		inline auto slice(size_t start, size_t count) -> span<T>
		{
			assert(start + count <= this->length_);

			return { this->ptr_ + start, count };
		}

	private:
		T* ptr_;
		size_t length_;
	};

	template <typename T> class linked_list;

	template <typename T> class linked_node
	{
	public:
		inline linked_node() : next_(nullptr), prev_(nullptr)
		{
		}

		inline auto next() -> T*
		{
			return static_cast<T*>(this->next_);
		}

		inline auto next() const -> const T*
		{
			return static_cast<const T*>(this->next_);
		}

		inline auto prev() -> T*
		{
			return static_cast<T*>(this->prev_);
		}

		inline auto prev() const -> const T*
		{
			return static_cast<const T*>(this->prev_);
		}

		inline auto next_node() -> linked_node*&
		{
			return this->next_;
		}

		inline auto prev_node() -> linked_node*&
		{
			return this->prev_;
		}

		inline void disconnect()
		{
			if (this->prev_ != nullptr)
			{
				this->prev_->next_ = this->next_;
			}

			if (this->next_ != nullptr)
			{
				this->next_->prev_ = this->prev_;
			}
		}

		inline auto next_ref() const -> linked_node<T>* const*
		{
			return &this->next_;
		}

		inline auto prev_ref() const -> linked_node<T>* const*
		{
			return &this->prev_;
		}

	private:
		linked_node* next_;
		linked_node* prev_;

	private:
		friend class linked_list<T>;
	};

	template <typename T> class linked_list
	{
	public:
		inline linked_list()
		{
			this->head_.next_ = &this->head_;
			this->head_.prev_ = &this->head_;
		}

		inline void null()
		{
			this->head_.next_ = nullptr;
			this->head_.prev_ = nullptr;
		}

		inline auto head() -> T*
		{
			return static_cast<T*>(&this->head_);
		}

		inline auto head() const -> const T*
		{
			return static_cast<const T*>(&this->head_);
		}

		inline auto begin() -> T*
		{
			return this->head_.next();
		}

		inline auto begin() const -> const T*
		{
			return this->head_.next();
		}

		inline auto tail() -> T*
		{
			return this->head_.prev();
		}

		inline auto tail() const -> const T*
		{
			return this->head_.prev();
		}

		inline auto end() -> T*
		{
			return static_cast<T*>(&this->head_);
		}

		inline auto end() const -> const T*
		{
			return static_cast<const T*>(&this->head_);
		}

		inline bool empty() const
		{
			return this->head_.next_ == &this->head_;
		}

		inline void add(T* val)
		{
			linked_node<T>* node = static_cast<linked_node<T>*>(val);

			node->prev_ = this->head_.prev_;

			this->head_.prev_->next_ = node;

			this->head_.prev_ = node;

			node->next_ = &this->head_;
		}

		inline void remove(T* val)
		{
			linked_node<T>* node = static_cast<linked_node<T>*>(val);

			node->prev_->next_ = node->next_;
			node->next_->prev_ = node->prev_;
		}

		inline auto remove_first() -> T*
		{
			linked_node<T>* curr = this->head_.next_;

			curr->prev_->next_ = curr->next_;
			curr->next_->prev_ = curr->prev_;

			return static_cast<T*>(curr);
		}

		inline auto remove_last() -> T*
		{
			linked_node<T>* curr = this->head_.prev_;

			curr->prev_->next_ = curr->next_;
			curr->next_->prev_ = curr->prev_;

			return static_cast<T*>(curr);
		}

		inline void add_after(T* val, T* node)
		{
			linked_node<T>* val_node = static_cast<linked_node<T>*>(val);
			linked_node<T>* node_node = static_cast<linked_node<T>*>(node);

			val_node->prev_ = node_node;

			val_node->next_ = node_node->next_;

			node_node->next_->prev_ = val_node;

			node_node->next_ = val_node;
		}

		inline void add_before(T* val, T* node)
		{
			linked_node<T>* val_node = static_cast<linked_node<T>*>(val);
			linked_node<T>* node_node = static_cast<linked_node<T>*>(node);

			val_node->next_ = node_node;

			val_node->prev_ = node_node->prev_;

			node_node->prev_->next_ = val_node;

			node_node->prev_ = val_node;
		}

		inline void clear()
		{
			this->head_.prev_ = &this->head_;
			this->head_.next_ = &this->head_;
		}

		inline void move_front(T* val)
		{
			this->remove(val);
			this->add_before(val, this->begin());
		}

		inline void move_end(T* val)
		{
			this->remove(val);
			this->add_after(val, this->tail());
		}

		template <typename F> inline void foreach(F action)
		{
			for (T* i = this->begin(); i != this->end(); i = i->next())
			{
				action(i);
			}
		}

		template <typename F> inline void foreach_reverse(F action)
		{
			for (const T* i = this->tail(); i != this->end(); i = i->prev())
			{
				action(i);
			}
		}

		template <typename P> inline void sort(P predicate)
		{
			if (this->head_.next_ != &this->head_)
			{
				linked_node<T>* root = this->head_.next_;

				linked_node<T>* curr = root->next_;

				root->prev_ = &this->head_;
				root->next_ = &this->head_;

				this->head_.prev_ = root;
				this->head_.next_ = root;

				while (curr != &this->head_)
				{
					linked_node<T>* next = curr->next_;

					linked_node<T>* temp = root;

					while (temp != &this->head_)
					{
						if (predicate(static_cast<T*>(temp), static_cast<T*>(curr)) >= 0)
						{
							break;
						}

						temp = temp->next_;
					}

					curr->next_ = temp;

					curr->prev_ = temp->prev_;

					temp->prev_->next_ = curr;

					temp->prev_ = curr;

					curr = next;

					root = this->head_.next_;
				}
			}
		}

	private:
		linked_node<T> head_;
	};

	struct scenery_info
	{
		char debug_name[0x18];
		std::uint32_t solid_keys[4];
		void* models[4];
		float radius;
		std::uint32_t flags;
		std::uint32_t hierarchy_key;
		void* hierarchy;
	};

	struct scenery_instance
	{
		Vector bbox_min;
		uint32_t flags;
		Vector bbox_max;
		std::int16_t preculler_info_index;
		std::int16_t lighting_context_number;
		Vector position;
		shared::utils::vector::matrix3x3 rotation;
		std::uint32_t scenery_guid;
		std::int16_t scenery_info_number;
		std::int8_t lod_level;
		std::int8_t custom_flags;
		void* custom;
		void* light_tex_collection;
	};

	struct preculler_info
	{
		std::uint8_t visibility_bits[0x80];
	};

	struct scenery_pack : linked_node<scenery_pack>
	{
	public:
		std::uint32_t chunks_loaded;
		std::uint16_t section_number;
		std::uint32_t polygon_in_memory_count;
		std::uint32_t polygon_in_world_count;
		span<scenery_info> infos;
		span<scenery_instance> instances;
		void* ngbbs;
		span<tree_node> tree_nodes;
		span<preculler_info> preculler_infos;
		std::uint32_t views_visible_this_frame;
		void* light_tex_list;

	public:
		static inline linked_list<scenery_pack>& list = *reinterpret_cast<linked_list<scenery_pack>*>(0x00B70640);
	};

	// ----------------------------------
	// (c) github.com/MaxHwoy/hyperlinked

	struct info;

	struct render_state
	{
		/* 0b00000001 */ uint32_t z_write_enabled : 1;
		/* 0b00000002 */ uint32_t is_backface_culled : 1;
		/* 0b00000004 */ uint32_t alpha_test_enabled : 1;
		/* 0b00000008 */ uint32_t alpha_blend_enabled : 1;
		/* 0b00000010 */ uint32_t alpha_blend_src : 4;
		/* 0b00000100 */ uint32_t alpha_blend_dest : 4;
		/* 0b00001000 */ uint32_t texture_address_u : 2;
		/* 0b00004000 */ uint32_t texture_address_v : 2;
		/* 0b00010000 */ uint32_t has_texture_animation : 1;
		/* 0b00020000 */ uint32_t is_additive_blend : 1;
		/* 0b00040000 */ uint32_t wants_auxiliary_textures : 1;
		/* 0b00080000 */ uint32_t bias_level : 2;
		/* 0b00200000 */ uint32_t multi_pass_blend : 1;
		/* 0b00400000 */ uint32_t colour_write_alpha : 1;
		/* 0b00800000 */ uint32_t sub_sort_key : 1;
		/* 0b01000000 */ uint32_t alpha_test_ref : 4;
		/* 0b10000000 */ uint32_t padding : 4;
	};

	enum compression_type : uint8_t
	{
		default_ = 0,
		p4 = 4,
		p8 = 8,
		p16 = 16,
		p16_1555 = 17,
		p16_565 = 18,
		p16_3555 = 19,
		rgb24 = 24,
		rgba32 = 32,
		dxt = 33,
		dxt1 = 34,
		dxt3 = 36,
		dxt5 = 38,
		ati2 = 39, // ATI2
		l8 = 40,
		dxt1_air = 41,
		dxt1_aig = 42,
		dxt1_aib = 43,
		ati1 = 44, // ATI1
		p8_16 = 128,
		p8_64 = 129,
		p8_ia8 = 130,
		p4_ia8 = 131,
		p4_rgb24_a8 = 140,
		p8_rgb24_a8 = 141,
		p4_rgb16_a8 = 142,
		p8_rgb16_a8 = 143,
		invalid = 255,
	};

	enum lock_type : uint8_t
	{
		read = 0x0,
		write = 0x1,
		read_write = 0x2,
	};

	enum tileable_type : uint8_t
	{
		clamp = 0x00,
		u_repeat = 0x01,
		v_repeat = 0x02,
		u_mirror = 0x04,
		v_mirror = 0x08,
	};

	enum scroll_type : uint8_t
	{
		none = 0x00,
		smooth = 0x01,
		snap = 0x02,
		offset_scale = 0x03,
	};

	enum alpha_blend_type : uint8_t
	{
		src_copy = 0,
		blend = 1,
		additive = 2,
		subtractive = 3,
		overbright = 4,
		dest_blend = 5,
		dest_additive = 6,
		dest_subtractive = 7,
		dest_overbright = 8,
	};

	enum alpha_usage_type : uint8_t
	{
		none_ = 0,
		punchthru = 1,
		modulated = 2,
	};

	enum bit_flags : uint8_t
	{
		unknown = 0x01,
		disable_culling = 0x02,
	};

	struct page_range
	{
		float u0;
		float u1;
		float v0;
		float v1;
		uint32_t flags;
		uint32_t key;
		uint32_t pad1;
		uint32_t pad2;
	};

	struct platform_info
	{
		int pad1;
		int pad2;
		render_state state;
		uint32_t type;
		IDirect3DTexture9* texture;
		uint16_t punchthru_value;
		uint16_t format;
	};

	struct platform_interface
	{
		platform_info* pinfo;
	};

	struct texture_info : public platform_interface
	{
		int pad1;
		int pad2;
		uint32_t key;
		uint32_t class_key;
		int32_t image_placement;
		int32_t palette_placement;
		int32_t image_size;
		int32_t palette_size;
		int32_t base_image_size;
		uint16_t width;
		uint16_t height;
		uint8_t shift_width;
		uint8_t shift_height;
		compression_type image_compression_type;
		uint8_t palette_compression_type;
		uint16_t num_palette_entries;
		uint8_t num_mipmap_levels;
		tileable_type tilable_uv;
		uint8_t bias_level;
		uint8_t rendering_order;
		scroll_type scroll;
		uint8_t used_flag;
		uint8_t apply_alpha_sorting;
		alpha_usage_type usage_type;
		alpha_blend_type blend_type;
		bit_flags flags;
		int16_t scroll_time_step;
		int16_t scroll_speed_s;
		int16_t scroll_speed_t;
		int16_t offset_s;
		int16_t offset_t;
		int16_t scale_s;
		int16_t scale_t;
		int16_t unknown;
		struct pack* pack;
		void* image_data;
		void* weights;
		uint8_t name_len;
		char name[35];
	};

	struct index_entry
	{
		uint32_t key;
		texture_info* texture;
	};

	struct e_texture
	{
		int pad1;
		int pad2;
		uint32_t key;
		texture_info* texture;
	};

	struct pack_header
	{
		int32_t version;
		char name[28];
		char filename[64];
		uint32_t key;
		uint32_t perm_chunk_byte_offset;
		uint32_t perm_chunk_byte_size;
		bool endian_swapped;
		__declspec(align(0x04)) struct pack* pack;
		index_entry* texture_index_entry_table;
		struct e_streaming_entry* texture_stream_entry_table;
	};

	struct vram_data_header
	{
		int pad1;
		int pad2;
		uint32_t version;
		uint32_t key;
		bool endian_swapped;
		void* vram_data_chunk;
	};

	struct pack
	{
		int pad1;
		int pad2;
		const char* name;
		const char* filename;
		uint32_t key;
		pack_header* header;
		uint32_t textures_count;
		uint32_t texture_data_size;
		e_texture* next;
		e_texture* prev;
	};

	enum time_base : uint8_t
	{
		world,
		real,
	};

	struct plat
	{
		IDirect3DTexture9* attached;
	};

	struct entry
	{
		uint32_t key;
		texture_info* texture;
		plat* data;
	};

	struct animation
	{
		int pad1;
		int pad2;
		char name[16];
		uint32_t key;
		uint8_t frame_count;
		uint8_t fps;
		time_base base;
		uint8_t pad0;
		bool endian_swapped;
		bool valid;
		uint8_t current_frame;
		uint8_t pad3;
		void* parent_pack;
		entry* table;
	};

	struct color32
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	};

	struct vertex
	{
		Vector position;
		color32 color;
		Vector2D uv;
	};

	struct strip
	{
		uint32_t vertex_count;
		vertex vertices[0x40];
	};

	enum shader_type : uint32_t
	{
		WorldShader,
		WorldReflectShader,
		WorldBoneShader,
		WorldNormalMap,
		CarShader,
		CARNORMALMAP,
		WorldMinShader,
		FEShader,
		FEMaskShader,
		FilterShader,
		ScreenFilterShader,
		RainDropShader,
		VisualTreatmentShader,
		WorldPrelitShader,
		ParticlesShader,
		skyshader,
		shadow_map_mesh,
		CarShadowMapShader,
		WorldDepthShader,
		shadow_map_mesh_depth,
		NormalMapNoFog,
		InstanceMesh,
		ScreenEffectShader,
		HDRShader,
		UCAP,
		GLASS_REFLECT,
		WATER,
		RVMPIP,
		GHOSTCAR,
		count,
		first = 0,
	};

	struct mesh_entry
	{
		Vector bbox_min;
		Vector bbox_max;
		uint8_t diffuse_texture_index;
		uint8_t normal_texture_index;
		uint8_t height_texture_index;
		uint8_t specular_texture_index;
		uint8_t opacity_texture_index;
		uint8_t light_material_table_index;
		unsigned char blend_matrix_indices[16];
		uint16_t padding;
		shader_type type;
		effect* effect;
		uint32_t flags;
		uint32_t material_key;
		uint32_t vertex_count;
		char* chunk_data; // #TODO
		uint32_t chunk_data_size;
		uint32_t unknown_4;
		uint32_t unknown_5;
		uint32_t unknown_6;
		uint32_t unknown_7;
		uint32_t unknown_8;
		uint32_t triangle_count;
		uint32_t index_start;
		uint32_t idk_yet;
		void* file_vertex_buffer;
		uint32_t file_vertex_buffer_size;
		IDirect3DVertexBuffer9* d3d_vertex_buffer;
		uint32_t d3d_vertex_count;
		uint32_t index_count;
		const char* vlt_material_name;
		uint32_t overriden_material_key;
		void* vertex_buffer_data;
		uint32_t vertex_offset;
	};

	enum draw_flags : uint32_t
	{
		fully_visible = 1u << 2,

		rear_view_drawn = 1u << 8,
		always_facing = 1u << 9,
		render_ai_npc = 1u << 10,
		render_ai_racer = 1u << 11,
		some_flag_0x1000 = 1u << 12,
		chopped_roadway = 1u << 13,

		cast_shadows = 1u << 16,
		sky_shade = 1u << 17,
		inverted_culling = 1u << 18,
		dynamic_placement = 1u << 19,
		reflect_in_ocean = 1u << 20,

		high_quality = 1u << 22,
		dont_receive_shadows = 1u << 23,
		envmap_shadow = 1u << 24,
		use_ghost_shader = 1u << 25,

		dont_fix_replacement_tex = 1u << 31,
	};

	struct rendering_model
	{
		render_state render_bits;
		texture_info* base_texture_info;
		IDirect3DTexture9* d3d9_diffuse_texture;
		IDirect3DTexture9* d3d9_normal_texture;
		IDirect3DTexture9* d3d9_height_texture;
		IDirect3DTexture9* d3d9_specular_texture;
		IDirect3DTexture9* d3d9_opacity_texture;
		union {
			mesh_entry* entry;
			strip* strip;
		} mesh;
		bool is_tri_stripped;
		void* solid; // geometry::solid
		draw_flags flags;
		effect* effect;
		const void* light_context; // light::context::dynamic
		const void* light_material; // light_material::instance
		const D3DXMATRIX* local_to_world;
		const D3DXMATRIX* blending_matrices;
		const texture_info* diffuse_texture_info;
		const texture_info* normal_texture_info;
		const texture_info* height_texture_info;
		const texture_info* specular_texture_info;
		const texture_info* opacity_texture_info;
		int32_t sort_flags;
		void* null;
		float negative_one;
		void* blend_data; // pca::blend_data
		bool use_low_lod;
	};
}
