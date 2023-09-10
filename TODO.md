# TODO
## Vital
- [ ] 2023-9-10 - LJYC: Add RenderPass->Recreate() function to support Customized Passes.
- [ ] 2023-9-5  - LJYC: Implement ZReverse.
- [ ] 2023-9-2  - LJYC: Render to Image(Render Target) and simplify the creation of Framebuffer in Render Pass. And use Blit in surface pass to blit image to Swapchain for presentation

## Optimization
- [ ] 2023-9-2  - LJYC: GRI manage Global Shaders.
- [ ] 2023-9-1  - LJYC: Move commandbuffers to each Render Pass?

## Trival
- [ ] 2023-7-30 - LJYC: Solve VMA Implement macro issue (now two RHI function difinitions in GRI.cc)

## DONE
- [X] 2023-9-4  - Add a hashmap to ConvertLayout()
- [X] 2023-9-3  - LJYC: Modify Pipeline Cache path to APP asset directory.
- [X] 2023-9-2  - LJYC: Fix Descriptor Set Layout duplication issue in pipeline.