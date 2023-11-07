# TODO
## Vital
- [ ] 2023-11-07 - Y Inversion with Vulkan Extension.
- [ ] 2023-11-01 - Wait KTX 4.2.2 for integrate it with VMA.
- [ ] 2023-10-31 - Add KTX Texture definition, e.g., VK_FORMAT_B8G8R8A8_SRGB
- [ ] 2023-10-22 - Solve subresouce for Mipmap Levels.

## Optimization
- [ ] 2023-09-12 - Using a Global CommandBuffer of Transfer Queue.
- [ ] 2023-09-02 - GRI manage Global Shaders.
- [ ] 2023-09-01 - Move commandbuffers to each Render Pass?

## Trival

## DONE
- [X] 2023-10-22 - Dynamic Uniform Buffers.
- [X] 2023-09-05 - Implement ZReverse.
- [X] 2023-07-30 - Solve VMA Implement macro issue (now two RHI function difinitions in GRI.cc)
- [X] 2023-09-04 - Add a hashmap to ConvertLayout()
- [X] 2023-09-03 - Modify Pipeline Cache path to APP asset directory.
- [X] 2023-09-02 - Render to Image(Render Target) and simplify the creation of Framebuffer in Render Pass. And use Blit in surface pass to blit image to Swapchain for presentation
- [X] 2023-09-02 - Fix Descriptor Set Layout duplication issue in pipeline.